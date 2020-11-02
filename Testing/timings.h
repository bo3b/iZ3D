#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/filesystem/path.hpp>

// May be provide some more extensible way to setup gnuplot dir
#define GNUPLOT_PATH "..\\..\\lib\\gnuplot\\binary\\gnuplot.exe"

inline double getTimeInSeconds()
{
	LARGE_INTEGER counter, frequency;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&frequency);
    return static_cast<double>(counter.QuadPart) / frequency.QuadPart;
}

/** Get time interval from previous getTimeInterval call */
inline double getTimeInterval()
{
	static double time = getTimeInSeconds();
	double prevTime = time;
	time = getTimeInSeconds();
	return time - prevTime;
}

class performance_test
{
friend class test;
public:
	class performance;

	struct test
	{
	friend class performance;
	public:
		test(performance_test& pt_, const std::string& name_)
		:	name(name_)
		,	pt(pt_)
		,	performances(pt.get_num_competitors(), -1)
		{
			std::cout << "Testing '" << name << "':\n";
		}

		~test()
		{
			pt.add_test_result( name, 
								absciss, 
								performances.begin(), 
								performances.end() );
		}

	public:
		std::string	name;
		double		absciss;

	private:
		performance_test&	pt;
		std::vector<double>	performances;
	};
	
	class performance
	{
	public:
		performance(test& t_, const std::string& competitor_)
		:	t(t_)
		,	competitor(competitor_)
		{
			time = getTimeInSeconds();
		}

		~performance()
		{
			size_t competitorId = t.pt.get_competitor_id(competitor);
			if ( competitorId >= t.performances.size()  ) {
				t.performances.resize(competitorId + 1);
			}
			t.performances[competitorId] = getTimeInSeconds() - time;

			std::cout << competitor << ": " << t.performances[competitorId] << "s\n";
		}

	private:
		test&		t;
		double		time;
		std::string competitor;
	};

private:
	struct test_result
	{
		std::string			name;
		double				absciss;
		std::vector<double>	performances;
	};

	typedef std::vector<std::string> competitor_vector;
	typedef std::vector<test_result> test_result_vector;

public:
	performance_test(const std::string& name_)
	:	name(name_)
	{
	}

	void plot(const std::string& image, const std::string& abscissaName = "")
	{
		namespace fs = boost::filesystem;

		std::ofstream data_os("tmp.dat");
		std::ofstream plot_os("tmp.plot");

		size_t cnt = 0;
		
		for (auto i = testResults.begin(); i != testResults.end(); ++i, ++cnt)
		{
			if ( !abscissaName.empty() ) {
				data_os << i->absciss << " ";
			}
			else {
				data_os << cnt << " ";
			}

			for (auto j = i->performances.begin(); j != i->performances.end(); ++j) {
				data_os << *j << " ";
			}
			data_os << std::endl;
		}
		data_os.close();

		plot_os << "set terminal " << fs::path(image).extension().substr(1) << ";\n";
		plot_os << "set output '" << image << "';\n";
		plot_os << "set title \"" << name << "\";\n";
		plot_os << "set xlabel \"";
		if ( !abscissaName.empty() ) {
			plot_os << abscissaName << "\";\n";
		}
		else {
			plot_os << "test num;\"\n";
		}
		plot_os << "set ylabel \"time\";\n";
		plot_os << "set autoscale;\n";
			
		plot_os << "plot ";
		cnt = 0;
		for (size_t i = 0; i < competitors.size(); ++i)
		{
			plot_os << "\"tmp.dat\" using 1:" << i + 2 << " title '" << competitors[i] << "' with lines";
			if (i != competitors.size() - 1) {
				plot_os << ", ";
			}
			else {
				plot_os << ";\n";
			}
		}
		plot_os << "set output;\n";
		plot_os.close();

		// run gnuplot
		std::string cmd = std::string("\"") + GNUPLOT_PATH + "\" tmp.plot";
		system( cmd.c_str() );
	}

	size_t get_num_competitors() const 
	{
		return competitors.size();
	}

	size_t get_competitor_id(const std::string& competitor)
	{
		auto iter = std::find( competitors.begin(), competitors.end(), competitor);
		if (iter == competitors.end() ) 
		{
			competitors.push_back(competitor);
			return competitors.size() - 1;
		}

		return std::distance(competitors.begin(), iter);
	}

protected:
	template<typename Iterator>
	void add_test_result(const std::string& name,
						 double				absciss,
						 Iterator			begin,
						 Iterator			end)
	{
		test_result r;
		r.absciss = absciss;
		r.name = name;
		std::copy( begin, end, std::back_inserter(r.performances) );
		testResults.push_back(r);
	}

protected:
	std::string			name;
	competitor_vector	competitors;
	test_result_vector	testResults;
};
