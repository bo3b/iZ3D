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
#include "stdafx.h"
#include <d3dx9.h>
#include "Direct3DShader8.h"

HRESULT CompileVertexShader(DWORD* pHandle, LPTSTR sstream, IUnknown * pDev);

int GetVSDecl( VSDeclData &declData, char* vsDecl )
{
	int textsz = 0;
	std::vector<DWORD> ind;
#define MAXSTREAMS 16
#define MAXVEC 18
	// TODO: Optimize that
	DWORD cnt = 0;
	for (DWORD s = 0; s < MAXSTREAMS && cnt < declData.reg_vec.size(); s++)
	{
		for (DWORD v = 0; v < MAXVEC; v++)
		{
			int i = 0;
			int j = 0;
			D3DVERTEXELEMENT9* pVE = &declData.pVertexElements[i];
			while (pVE->Stream != 0xFF)
			{
				if (pVE->Method == D3DDECLMETHOD_DEFAULT)
				{
					if (s == pVE->Stream && v == declData.reg_vec[j])
					{
						DWORD rv = declData.reg_vec[j];
						switch(pVE->Usage)
						{
						case D3DDECLUSAGE_POSITION:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_position%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_BLENDWEIGHT:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_blendweight%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_BLENDINDICES:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_blendindices%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_NORMAL:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_normal%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_COLOR:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_color%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_PSIZE:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_psize%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						case D3DDECLUSAGE_TEXCOORD:
							textsz += sprintf( vsDecl+textsz,  "%sdcl_texcoord%ld v%lu%s",id, pVE->UsageIndex,rv,nl);
							break;
						default:
							break;
						}
						cnt++;
					}
					j++;
				}
				i++;
				pVE = &declData.pVertexElements[i];
			}
		}
	}
	for (DWORD i = 0; i < declData.const_vec.size(); i++)
	{
		CONSTANT_REGISTER* pCR = &declData.const_vec[i];
		textsz += sprintf( vsDecl+textsz,  "%sdef c%d, %f, %f, %f, %f%s",id, pCR->index,pCR->x,pCR->y,pCR->z,pCR->w,nl);
	}

	return textsz;
}

inline LONG GetSizeOfType( BYTE type )
{
	switch(type)
	{
	case D3DVSDT_FLOAT1:
		return sizeof(FLOAT);
	case D3DVSDT_FLOAT2:
		return sizeof(FLOAT)*2;//texcoord
	case D3DVSDT_FLOAT3:
		return sizeof(FLOAT)*3;
	case D3DVSDT_FLOAT4:
		return sizeof(FLOAT)*4;
	case D3DVSDT_D3DCOLOR://color
	case D3DVSDT_UBYTE4:
		return sizeof(BYTE)*4;//blendindexes
	case D3DVSDT_SHORT2:
		return sizeof(SHORT)*2;
	case D3DVSDT_SHORT4:
		return sizeof(SHORT)*4;
	default:
		DEBUG_TRACE1("\tUnknown vertex type\n");
		return 0;
	}
}

inline void GetVertexElementUsage(D3DVERTEXELEMENT9 &elem, DWORD registerAdr)
{
	switch(registerAdr)
	{
	case D3DVSDE_POSITION:
		elem.Usage = D3DDECLUSAGE_POSITION;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_POSITION2:
		elem.Usage = D3DDECLUSAGE_POSITION;
		elem.UsageIndex = 1;
		break;
	case D3DVSDE_BLENDWEIGHT:
		elem.Usage = D3DDECLUSAGE_BLENDWEIGHT;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_BLENDINDICES:
		elem.Usage = D3DDECLUSAGE_BLENDINDICES;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_NORMAL:
		elem.Usage = D3DDECLUSAGE_NORMAL;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_NORMAL2:
		elem.Usage = D3DDECLUSAGE_NORMAL;
		elem.UsageIndex = 1;
		break;
	case D3DVSDE_PSIZE:
		elem.Usage = D3DDECLUSAGE_PSIZE;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_DIFFUSE:
		elem.Usage = D3DDECLUSAGE_COLOR;
		elem.UsageIndex = 0;
		break;
	case D3DVSDE_SPECULAR:
		elem.Usage = D3DDECLUSAGE_COLOR;
		elem.UsageIndex = 1;
		break;
	case D3DVSDE_TEXCOORD0:
	case D3DVSDE_TEXCOORD1:
	case D3DVSDE_TEXCOORD2:
	case D3DVSDE_TEXCOORD3:
	case D3DVSDE_TEXCOORD4:
	case D3DVSDE_TEXCOORD5:
	case D3DVSDE_TEXCOORD6:
	case D3DVSDE_TEXCOORD7:
		elem.Usage = D3DDECLUSAGE_TEXCOORD;
		elem.UsageIndex = (BYTE)(registerAdr - D3DVSDE_TEXCOORD0);
		break;
	default:
		DEBUG_MESSAGE("\tUnknown element type\n");
		break;
	}    
}

HRESULT ConvertVSDecl( CONST DWORD* pDeclaration, DWORD &declSize, VSDeclData &declData, char* buffer  )
{
	HRESULT hr = S_OK;

	int cn = 0;
#ifndef FINAL_RELEASE
#define ADD2BUFFER cn += sprintf
#else
#define ADD2BUFFER
#endif
	declSize = 0;
	if(pDeclaration){
		std::vector<D3DVERTEXELEMENT9> elem_vec;
		LONG strm_shift = 0;
		
		DWORD* pdwItem = (DWORD*)pDeclaration;
		INT cur_strm = -1;
		ADD2BUFFER( buffer + cn, "\tD3D8 Vertex Declaration\n");
		ADD2BUFFER( buffer + cn, "\t{\n");
		declSize += sizeof(DWORD);
		while ((*pdwItem)!=D3DVSD_END())
		{
			std::vector<DWORD> vec;
			D3DVSD_TOKENTYPE tokenType = (D3DVSD_TOKENTYPE)(((*pdwItem) & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT);
			switch(tokenType)
			{
			case D3DVSD_TOKEN_NOP:
				ADD2BUFFER( buffer + cn, "\t\tD3DVSD_NOP(),\n");
				break;
			case D3DVSD_TOKEN_STREAM: // Stream Selector
				{
					DWORD stream = ((*pdwItem) & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT;
					BOOL dataLoadType = ((*pdwItem) & D3DVSD_DATALOADTYPEMASK) >> D3DVSD_DATALOADTYPESHIFT; // 1 if tesselator stream
					if (dataLoadType == 0) {
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_STREAM(%u),\n", stream);
						cur_strm = stream;
						strm_shift = 0;
					} else {
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_STREAM_TESS(),\n");
					}
				}
				break;
			case D3DVSD_TOKEN_STREAMDATA: // Stream Data Definition
				{
					DWORD dataLoadType = ((*pdwItem) & D3DVSD_DATALOADTYPEMASK) >> D3DVSD_DATALOADTYPESHIFT;
					D3DVERTEXELEMENT9 elem;
					memset(&elem,0,sizeof(elem));
					elem.Stream = cur_strm;
					elem.Offset = (WORD)strm_shift;
					elem.Method = D3DDECLMETHOD_DEFAULT;
					if(dataLoadType == 0) // Vertex Input Register Load
					{
						elem.Type = (BYTE)(((*pdwItem) & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT);
						DWORD registerAdr = ((*pdwItem) & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT; 
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_REG(%s(v%u), %s),\n", GetVSDEString(registerAdr), registerAdr, GetVSDTString(elem.Type));
						declData.reg_vec.push_back(registerAdr);
						GetVertexElementUsage(elem, registerAdr);
						strm_shift += GetSizeOfType(elem.Type);
						elem_vec.push_back(elem);
					}
					else // Data Skip (no register load)
					{
						DWORD skipCount = ((*pdwItem) & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_SKIP(%u),\n", skipCount); 
						strm_shift += skipCount * 4;
					}
				}
				break;
			case D3DVSD_TOKEN_TESSELLATOR: // vertex input memory from tessellator
				{
					DWORD dataLoadType = ((*pdwItem) & D3DVSD_DATALOADTYPEMASK) >> D3DVSD_DATALOADTYPESHIFT;
					DWORD registerAdrOut = ((*pdwItem) & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT; 
					DWORD dataType = ((*pdwItem) & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT; 
					DWORD registerAdrIn = ((*pdwItem) & D3DVSD_VERTEXREGINMASK) >> D3DVSD_VERTEXREGINSHIFT; 
					D3DVERTEXELEMENT9 elem;
					memset(&elem,0,sizeof(elem));
					if(dataLoadType == 0) // normals
					{ 
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_TESSNORMAL(VertexRegisterIn=%u, Usage=%u),\n", registerAdrIn, registerAdrOut);
						elem.Method = D3DDECLMETHOD_CROSSUV;
						elem.Type = (BYTE)dataType;
						declData.reg_vec.push_back(registerAdrOut);
						elem.Usage = (BYTE)registerAdrOut;
						for (DWORD i=1; i<=elem_vec.size(); i++)
						{
							if((elem_vec[elem_vec.size()-i].Stream == cur_strm)
								&&(elem_vec[elem_vec.size()-i].Usage == registerAdrIn)){
									elem.Stream = elem_vec[elem_vec.size()-i].Stream;
									elem.Offset = elem_vec[elem_vec.size()-i].Offset;
									elem_vec.push_back(elem);
									break;
							}
						}
					}
					else// u/v
					{
						ADD2BUFFER( buffer + cn, "\t\tD3DVSD_TESSUV(VertexRegister=%u),\n", registerAdrOut);
						elem.Stream = 0;
						elem.Offset = 0;
						elem.Method = D3DDECLMETHOD_UV;
						elem.Type = D3DDECLTYPE_UNUSED;
						
						elem.Usage = (BYTE)registerAdrOut;
						declData.reg_vec.push_back(registerAdrOut);
						GetVertexElementUsage(elem, registerAdrOut);
						elem_vec.push_back(elem);
					}
				}
				break;
			case D3DVSD_TOKEN_CONSTMEM: // Constant Memory from Shader (def)
				{
					DWORD constCount = (((*pdwItem)&D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT);
					DWORD constAdr    = (((*pdwItem) & D3DVSD_CONSTADDRESSMASK) >> D3DVSD_CONSTADDRESSSHIFT);
					ADD2BUFFER( buffer + cn, "\t\tD3DVSD_CONST(ConstantAddress=%u, Count=%u),\n", constAdr, constCount); 
					for (size_t i = 0; i < constCount; i++)
					{
						CONSTANT_REGISTER reg;
						reg.index = constAdr + i;
						pdwItem ++;
						reg.x = *((float*)pdwItem);
						pdwItem ++;
						reg.y = *((float*)pdwItem);
						pdwItem ++;
						reg.z = *((float*)pdwItem);
						pdwItem ++;
						reg.w = *((float*)pdwItem);
						ADD2BUFFER( buffer + cn, "\t\t\tc%i: x = %f\ty = %f\tz = %f\tw = %f\n", reg.index, reg.x, reg.y, reg.z, reg.w);
						declData.const_vec.push_back(reg);
					}
				}
				break;
			case D3DVSD_TOKEN_EXT: // Extension Token
				{
					DWORD extCount = (((*pdwItem)&D3DVSD_EXTCOUNTMASK) >> D3DVSD_EXTCOUNTSHIFT);
					DWORD extInfo    = (((*pdwItem) & D3DVSD_EXTINFOMASK) >> D3DVSD_EXTINFOSHIFT); 
					ADD2BUFFER( buffer + cn, "\t\tExtension\n"); 
				}
				break;
			default:
				break;
			}
			pdwItem++;
			declSize += sizeof(DWORD);
		}
		D3DVERTEXELEMENT9 end_elem = D3DDECL_END();
		ADD2BUFFER( buffer + cn, "\t\tD3DVSD_END()\n");
		elem_vec.push_back(end_elem);
		ADD2BUFFER( buffer + cn, "\t}\n\n");
		declData.pVertexElements = DNew D3DVERTEXELEMENT9[elem_vec.size()];
		ADD2BUFFER( buffer + cn, "\tD3DVERTEXELEMENT9\n");
		ADD2BUFFER( buffer + cn, "\t{\n");
		for(DWORD i = 0; i<elem_vec.size(); i++){
			declData.pVertexElements[i] = elem_vec[i];
			if (declData.pVertexElements[i].Stream != 0xFF)
				ADD2BUFFER( buffer + cn, "\t\t{ Stream=%d, Offset=%d, Type=%s, Method=%s, Usage=%s, UsageIndex=%d },\n",
				elem_vec[i].Stream, elem_vec[i].Offset, GetVEDTypeString(elem_vec[i].Type), GetVEDMethodString(elem_vec[i].Method), 
				GetVEDUsageString(elem_vec[i].Usage), elem_vec[i].UsageIndex);
			else
				ADD2BUFFER( buffer + cn, "\t\tD3DDECL_END()\n");
		}
		ADD2BUFFER( buffer + cn, "\t}\n");
	};
	return hr;
#undef ADD2BUFFER
}

DWORD shaderGlobalIndex = 0;

void WriteShaderTextToFile(TCHAR* fileName, char* shaderText, char* buffer)
{
#ifndef FINAL_RELEASE

	if(!shaderText) 
		return;

	FILE * fp;

	if (shaderGlobalIndex == 0) 
		fp = _tfopen( fileName, _T("w") );
	else 
		fp = _tfopen( fileName, _T("a") );
	if ( fp ) 
	{
		fprintf( fp, "\n;*******************************************************************************\n; "
			"Vertex Shader #%u\n%s\n", shaderGlobalIndex, shaderText);
		if (buffer)
			fprintf( fp, buffer);
		fclose( fp );
	}

#endif
}

void WriteErrorTextToFile(TCHAR* fileName, char* errorText)
{
#ifndef FINAL_RELEASE

	if(!errorText) 
		return;

	FILE * fp;

	if (shaderGlobalIndex == 0) 
		fp = _tfopen( fileName, _T("w") );
	else 
		fp = _tfopen( fileName, _T("a") );
	if ( fp ) 
	{
		fprintf( fp, "\n%s\n", errorText);
		fclose( fp );
	}

#endif
}

inline char * getNextLine( char* text )
{
	char* s = strchr( text, '\n');
	if (!s)
		s = text + strlen(text);
	else if (s[0] == 10)
		s++;
	return s;
}

bool FindLine(char* &text, DWORD &curLine, DWORD lineNumber)
{            
	for(; curLine < lineNumber; curLine++)
	{
		text = getNextLine(text);
		if (!text || text[0] == '\0')
			return false;
	}
	return true;
}

bool GetErrorCode(char* &errorMsg, DWORD &errorLine, DWORD &errorCode)
{
	if(errorMsg && errorMsg[0] != '\0')
	{
		errorCode = 0;
		char* nextLine = getNextLine(errorMsg);
		char* begin = strstr(errorMsg, "error X");
		if(begin && begin < nextLine)
		{
			char temp[10];
			begin = strchr(errorMsg, '(') + 1;
			char* end = strchr(begin, ')');
			char* p = strchr(begin, ',');
			if (p && p < end)
				end = p;
			strncpy_s(temp, 10, begin, end - begin );
			errorLine = atoi(temp);

			begin = strstr(begin, "error X");
			end = strchr(begin, ':');
			if (begin && end)
				strncpy_s(temp, 10, begin + 7, end - begin - 7 );
			errorCode = atoi(temp);
			errorMsg = end + 1;
		}
		return true;
	};
	return false;
}

char *strshift(char *strDest, const char *strSource)
{
	size_t count = strlen(strSource) + 1;
	if (strDest > strSource)
	{
		const char* s = strSource + count - 1;
		char* d = strDest + count - 1;
		for(size_t i = 0; i < count; i++, s--, d--)
			d[0] = s[0];

		//d = strDest;		
		//count = strDest - strSource;
		//for(size_t i = 0; i < count; i++, d++)
		//	d[0] = ' ';
	}
	else
	{
		const char* s = strSource;
		char* d = strDest;		
		for(size_t i = 0; i < count; i++, s++, d++)
			d[0] = s[0];
	}
	return strDest;
}

DWORD PreprocessVS( char* errorMsg, LPSTR sstream)
{
	DWORD errorFixed = 0;
	DWORD curLine = 1;
	char* shaderText = sstream;

	const char* scalarOutput[2] = { "oFog", "oPts" };
	DWORD errorCode = 0;
	DWORD errorLineNumber = 0;
	while (GetErrorCode(errorMsg, errorLineNumber, errorCode))
	{
		if (!FindLine(shaderText, curLine, errorLineNumber))
			break;
		char* nextErrorLine = getNextLine(errorMsg);
		char* nextLine = getNextLine(shaderText);
		switch(errorCode)
		{
		case 5326:
			{
				//mul r4, r4, v1.x

				char* u = strchr(errorMsg, '*');
				char* begin = strstr(u, " in ") + 4;
				char* end = strchr(begin, ':');
				char reg[10];
				strncpy_s(reg, 10, begin, end - begin );
				u = strchr(end + 1, '*');
				DWORD comp = 0;
				while(u) 
				{
					switch(u[1])
					{
					case 'x':
						comp |= 1;
						break;
					case 'y':
						comp |= 2;
						break;
					case 'z':
						comp |= 4;
						break;
					case 'w':
						comp |= 8;
						break;
					}
					u = strchr(u + 1, '*');
				};

				char *str = strstr(shaderText, reg);
				char *comma = strchr(shaderText, ',');
				if (comma && comma > str)
				{
					str = strstr(comma, reg);
				}
				if(str && str < nextLine)
				{
					shaderText = str;
					str += strlen(reg);
					char c[6]= "";
					if (str[0] == '.')
					{
						char s[5] = "xyzw";
						strncpy_s(c, str + 1, strspn(str + 1, "xyzwrgba"));
						size_t delSymbols = 0;
						for(int p = 0; p < 4; p++)
						{
							if ((1 << p) & comp)
							{
								size_t t = 0;
								for(size_t i = 0; i < strlen(c); i++)
								{
									if (c[i] == s[p])
										t++;
									if (i + t < strlen(c))
										c[i] = c[i+t];
									else
										c[i] = ' ';
								}
								delSymbols += t;
							}
						}
						strncpy(str + 1, c, strlen(c));
						if (delSymbols >= strlen(c))
							str[0] = ' ';

						shaderText = str + 1 + strlen(c);
						errorFixed++;
					}
					else
					{
						strcat(c, ".");
						if (comp & ~1)
							strcat(c, "x");
						if (comp & ~2)
							strcat(c, "y");
						if (comp & ~4)
							strcat(c, "z");
						if (comp & ~8)
							strcat(c, "w");
						size_t cnt = strlen(c);
						strshift(str + cnt, str);
						strncpy(str, c, cnt);
						shaderText = str + strlen(c);
						errorFixed++;
					}
				}
			}
			break;
		case 5430:
			//(16): error X5430: When writing to scalar output register, mov instruction must use replicate swizzle on source parameter(s), in order to select single component. i.e. .x | .y | .z | .w (or rgba equivalent)
			// mov sge rsq rcp
			// sge oFog.x, c0, c0
			for (int i = 0; i < 2; i++)
			{
				char* pos = strstr(shaderText, scalarOutput[i]);
				if (pos && pos < nextLine)
				{
					for (int j = 0; j < 2; j++)
					{
						char* p = strstr(pos, "  , ");
						if (p)
						{
							size_t t = strspn(p + 5, "0123456789");
							if (t > 0)
							{
								strncpy(p, ", ", 2);
								strncpy(p + 2, p + 4, 1 + t);
								strncpy(p + 3 + t, ".x", 2);
								pos = p + 5 + t;
								if (j == 0)
									errorFixed++;
							}
						}
						else
						{
							p = strstr(pos, ", ");
							if (p)
							{
								size_t t = strspn(p + 3, "0123456789");
								if (t > 0)
								{
									strshift(p + 5 + t, p + 3 + t);
									strncpy(p + 3 + t, ".x", 2);
									pos = p + 5 + t;
									if (j == 0)
										errorFixed++;
								}
							}
						}
					}
					break;
				}
			}
			break;
		case 5485:
			//(10): error X5485: rsq requires replicate swizzle for the source parameter in order to select component. i.e. .x | .y | .z | .w (or rgba equivalent)
			// rsq rcp
			// rsq r1.xyz, r1.xyzz
			{
				char* pos = strstr(shaderText, ".xyzz");
				if (pos && pos < nextLine)
				{
					char* c = pos + 2;
					for (int i = 0; i < 3; i++, c++)
						c[0] = ' ';
					errorFixed++;
				}
			}
			break;
		case 5350: 
			//(18): error X5350: Vertex shader must minimally write all four components (xyzw) of oPos output register.  Missing component(*): x/0 y/1 z/2 *w/3
			//mov oPos.z, v0.z
			strcat(shaderText, "\nmov oPos.w, v0.w\n");
			errorFixed++;
			break;
		case 2022:
			//(16): error X2022: scalar registers cannot be masked
			for (int i = 0; i < 2; i++)
			{
				char* pos = strstr(shaderText, scalarOutput[i]);
				if (pos && pos < nextLine)
				{
					for (char* c = pos + 4; c[0] != ','; c++)
						c[0] = ' ';
					errorFixed++;
				}
			}
			break;
		default:
			break;
		}	
		errorMsg = nextErrorLine;
	}
	return errorFixed;
}

HRESULT CompileVertexShader(IDirect3DVertexShader9** ppVS, LPSTR sstream, IDirect3DDevice9 * pDevice, char *buffer)
{
	if(!(sstream && ppVS))
		return E_FAIL;
	HRESULT hr;
	CComPtr<ID3DXBuffer> pCode;
	CComPtr<ID3DXBuffer> pErrorMsgs;
	*ppVS = 0;
	UINT streamLen = (UINT)strlen(sstream);
	hr = D3DXAssembleShader( sstream, streamLen, NULL, NULL, 0, &pCode, &pErrorMsgs);
	if (hr == D3DXERR_INVALIDDATA)
	{
		if (PreprocessVS((char*) pErrorMsgs->GetBufferPointer(), sstream) > 0)
		{
			pCode = NULL;
			pErrorMsgs = NULL;
			streamLen = (UINT)strlen(sstream);
			hr = D3DXAssembleShader( sstream, streamLen, NULL, NULL, 0, &pCode, &pErrorMsgs);
		}
	}
#ifndef FINAL_RELEASE
	if(GINFO_DEBUG) 
	{
		TCHAR shader_file[ MAX_PATH ];
		_tcscpy( shader_file, gInfo.LogFileName );
		_tcscpy( &shader_file[ _tcslen( shader_file ) - 4 ], _T("_shaders.d3d9.txt") );
		WriteShaderTextToFile(shader_file, sstream, buffer);
		if( FAILED(hr )){
			DEBUG_MESSAGE("%s\n", (char*)pErrorMsgs->GetBufferPointer());
			TCHAR shader_file[ MAX_PATH ];
			_tcscpy( shader_file, gInfo.LogFileName );
			_tcscpy( &shader_file[ _tcslen( shader_file ) - 4 ], _T("_shaders.d3d9.txt") );
			WriteErrorTextToFile(shader_file, (char*)pErrorMsgs->GetBufferPointer());
		}
	}
#endif
	if( FAILED(hr )){
		return hr;
	}
	hr = pDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), ppVS);
	return hr;
}
