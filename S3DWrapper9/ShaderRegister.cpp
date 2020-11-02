/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "StdAfx.h"
#include "ShaderRegister.h"
#include <stack>
#include <sstream>
#include <queue>

using namespace std;

const char *skipAlpha(const char *text)
{
	// skip all alpha while text not found
	while(*text && isalpha((int)*text))
		text++;
	return text;
}

const char *skipDigits(const char *text)
{
	// skip all digits while text not found
	while(*text && isdigit((int)*text))
		text++;
	return text;
}

int getSwizzleIndex(const char* reg)
{
	reg = skipDigits(skipAlpha(reg));
	const char* b = 0;
	if(*reg == '[')
		b = strrchr(reg, ']');
	const char* p = strchr(b ? b+1 : reg, '.');
	if(p)
	{
		switch(p[1])
		{
		case 'x':
		case 'r':
			return 0;
		case 'y':
		case 'g':
			return 1;
		case 'z':
		case 'b':
			return 2;
		case 'w':
		case 'a':
			return 3;
		}
	}
	return -1;
}

ShaderRegister::ShaderRegister() 
{
}

void ShaderRegister::SetCount( )
{
	switch (op[0])
	{
	case 'c':
		if(isdigit((int)op[1]))
		{	
			const char* s = skipDigits(op + 1);
			cRegCountWithInd++;
			if(s[0] != '[')
				cRegCount++;
		}
		break;
	case 'v':
		if(isdigit((int)op[1]))
		{
			vRegCount++;
		}
		break;
	}
}

bool ShaderRegister::GetCRegister( ScalarRegister& reg, bool skipIndexedConstantRegisters ) const
{
	if(isdigit((int)op[1]))
	{	
		//--- skip cX[a0.x] registers ---
		const char* s = skipDigits(op + 1);
		if(s[0] == '[' && skipIndexedConstantRegisters)
			return false;
		reg.registerNumber = atoi(op+1);
		reg.swizzleIndex = getSwizzleIndex(op);
		return true;
	}
	return false;
}

bool ShaderRegister::GetVRegister( ScalarRegister& reg ) const
{
	if(isdigit((int)op[1]))
	{
		reg.registerNumber = atoi(op+1);
		reg.swizzleIndex = getSwizzleIndex(op);
		return true;
	}
	return false;
}

void ShaderRegister::GetConstantRegistersList(vector<ScalarRegister> &s) const
{
	s.clear();
	treePos newPos;
	newPos.it = elem.begin();
	newPos.p = this;
	stack<treePos> allRegs;
	allRegs.push(newPos);
	do 
	{
		treePos& pos = allRegs.top();

		if (pos.it == pos.p->elem.begin())
		{
			ScalarRegister sr;
			if (pos.p->op[0] == 'c')
			{
				if(pos.p->GetCRegister(sr))
					s.push_back(sr);
			} 
		}

		if (pos.it != pos.p->elem.end())
		{
			treePos newPos;
			newPos.p = pos.it->reg;
			newPos.it = newPos.p->elem.begin();
			allRegs.push(newPos);
			++pos.it;
		} else
			allRegs.pop();
	} while(!allRegs.empty());
}

void ShaderRegister::ToString( std::string &str ) const
{
	str.clear();
	str.reserve(128);
	if (!elem.empty())
		str.append("(");
	treePos newPos;
	newPos.it = elem.begin();
	newPos.p = this;
	stack<treePos> allRegs;
	allRegs.push(newPos);
	do 
	{
		treePos& pos = allRegs.top();
		if (pos.it == pos.p->elem.begin())
			str.append(pos.p->op);

		if (pos.it != pos.p->elem.end())
		{
			treePos newPos;
			newPos.p = pos.it->reg;
			newPos.it = newPos.p->elem.begin();
			char op[2];
			op[0] = pos.it->comm;
			op[1] = '\0';
			str.append(op);
			if (!newPos.p->elem.empty())
				str.append("(");
			allRegs.push(newPos);
			++pos.it;
		} else
		{
			allRegs.pop();
			if (!pos.p->elem.empty())
				str.append(")");
		}
	} while(!allRegs.empty());
}

class TextIdent{
private:
	std::string str_;
public:
	TextIdent() : str_() {}
	void operator++ ( )
	{
		str_ += "  ";
	}
	void operator-- ( )
	{
		str_.erase(str_.size() - 2, 2);
	}
	friend std::ostream& operator << (std::ostream& os, const TextIdent& ident);
};

std::ostream& operator << (std::ostream& os, const TextIdent& ident)
{
	return os << ident.str_;
}

void ShaderRegister::ToTreeString( std::string &str ) const
{
	std::stringstream ss;
	TextIdent ident;
	if (!elem.empty())
		++ident;
	treePos newPos;
	newPos.it = elem.begin();
	newPos.p = this;
	char op[2];
	op[0] = ' ';
	op[1] = '\0';
	stack<treePos> allRegs;
	allRegs.push(newPos);
	do 
	{
		treePos& pos = allRegs.top();
		if (pos.it == pos.p->elem.begin())
		{
			if (pos.p->op[0])
				ss << ident << op << " " << pos.p->op << "\n";
			else
			{
				--ident;
				ss << ident << op << "\\" << "\n";
				++ident;
			}
		}

		if (pos.it != pos.p->elem.end())
		{
			treePos newPos;
			newPos.p = pos.it->reg;
			newPos.it = newPos.p->elem.begin();
			if (pos.it->comm != '\0')
				op[0] = pos.it->comm;
			else
				op[0] = ' ';
			if (!newPos.p->elem.empty())
				++ident;
			allRegs.push(newPos);
			++pos.it;
		} else
		{
			allRegs.pop();
			if (!pos.p->elem.empty())
				--ident;
		}
	} while(!allRegs.empty());
	str = ss.str();
}


bool ShaderRegister::OperationsMoreThanOne() const
{
	int opPlusCount = 0;
	int opMulCount = 0;
	queue<const ShaderRegister*> allRegs;
	allRegs.push(this);
	do 
	{
		const ShaderRegister* cur = allRegs.front();
		allRegs.pop();

		registersIterator it = cur->elem.begin(); 
		for (; it != cur->elem.end(); ++it)
		{
			char p = it->comm;
			if(p == '+')
			{
				opPlusCount++;
				if (opPlusCount > 1 && opMulCount > 1)
					return true;
			} else if(p == '*')
			{
				opMulCount++;
				if (opPlusCount > 1 && opMulCount > 1)
					return true;
			}
			if (!(*it).reg->elem.empty())
				allRegs.push(it->reg);
		}
	} while(!allRegs.empty());
	return false;
}

// function will return: true if success, outStart = '(' , outStop = ')'
bool ShaderRegister::GetNextBracketCouple(registersIterator &it, const RegisterWithOp* &subelement) const
{
	if (it != elem.end())
	{
		subelement = &(*it);
		++it;
		return true;
	}
	else
		return false;
}

bool ShaderRegister::SearchCoMultiplies(COMULTIPLIERS& outVal) const
{
	if(GetNextBracketCouple(outVal.it, outVal.s1))
	{
		if(GetNextBracketCouple(outVal.it, outVal.s2))
		{
			if (outVal.s2->comm == '*')
				return true;
		}
		else if (outVal.s1->comm == '+')
		{
			outVal.s2 = NULL;
			return true;
		}
	}
	return false;
}
