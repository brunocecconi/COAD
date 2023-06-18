
#define EASTL_ASSERT_ENABLED 0

#define BENCHMARK_STATIC_DEFINE

#define PRINT_ALLOCATIONS 0

#include <mimalloc.h>
#include <benchmark/benchmark.h>
#include <EASTL/unordered_map.h>
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/span.h>

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

void* operator new[](size_t size, const char*, int, unsigned, const char*, int)
{
	return malloc(size);
}

void* operator new[](size_t size, size_t, size_t, const char*, int, unsigned, const char*, int)
{
	return malloc(size);
}

void operator delete(void* ptr)
{
	free(ptr);
}

void operator delete[](void* ptr)
{
	free(ptr);
}

eastl::vector<eastl::string> First10000Words()
{
	FILE* lF = fopen("./english_words.txt", "r");
	eastl::vector<eastl::string> lWords{};
	lWords.reserve(100000);
	char lLine[64];
	int	 i = 0;
	while (fgets(lLine, sizeof lLine, lF) && i++ < 20000)
	{
		if (rand() % 2 == 0)
		{
			lWords.emplace_back(lLine);
		}
	}
	fclose(lF);
	return lWords;
}

eastl::string& RandomString(eastl::string& String)
{
	for (auto& C: String)
	{
		C = (rand() % 93) + 33;
	}
	return String;
}

uint64_t PhmHash(const char* Value, size_t Length)
{
	return eastl::hash<eastl::string_view>()(Value);
	if (Length < 2)
	{
		return eastl::hash<eastl::string_view>()(Value);
	}
	uint64_t lHash{};
	lHash = 31 * lHash + Value[0];
	lHash = 31 * lHash + Value[1];
	lHash = 31 * lHash + Value[2];
	lHash = 31 * lHash + Value[3];
	lHash = 31 * lHash + Value[Length - 1 - 0];
	lHash = 31 * lHash + Value[Length - 1 - 1];
	lHash = 31 * lHash + Value[Length - 1 - 2];
	lHash = 31 * lHash + Value[Length - 1 - 3];
	return lHash;
}

template<typename T, decltype(PhmHash) FunctionHash = PhmHash>
class PhmString
{
public:
	PhmString(const eastl::span<eastl::string>& Entries)
	{
		PopulateHash(Entries);
	}

	PhmString(std::initializer_list<eastl::string> Entries)
	{
		PopulateHash(eastl::span<eastl::string_view>{(eastl::string_view*)Entries.begin(), (eastl::string_view*)Entries.end()});
	}

	~PhmString()
	{
	}

public:
	void PopulateHash(const eastl::span<eastl::string>& Entries)
	{
		mCapacity = Entries.size();
		mHash.clear();
		mHash.resize(mCapacity);

		for (const auto& lEntry: Entries)
		{
			const auto lHash = FunctionHash(lEntry.data(), lEntry.size()) % mCapacity;
			mHash[lHash].Entries.reserve(16);
			mHash[lHash].Entries.emplace_back(lEntry, T{});
		}
	}

	T& operator[](eastl::string_view Key)
	{
		const auto lHash	= FunctionHash(Key.data(), Key.size()) % mCapacity;
		auto&	   lElement = mHash[lHash];
		if (lElement.Entries.size() == 1)
		{
			return lElement.Entries.front().Data;
		}
		else
		{
			for (auto& lEntry: lElement.Entries)
			{
				if (Key.front() == lEntry.Key.front() && Key == lEntry.Key)
				{
					return lEntry.Data;
				}
			}
		}
		// assert(false && "Not found element, or it has been collided.");
		static T d{};
		return d;
	}

private:
	struct EntryHash
	{
		eastl::string_view Key{};
		T				 Data{};
	};

	struct Element
	{
		eastl::vector<EntryHash> Entries{};
	};

	uint64_t			 mCapacity{};
	eastl::vector<Element> mHash{};
};

static void PhmStringTest(benchmark::State& state)
{
	eastl::vector<eastl::string> lStrings = First10000Words();
	PhmString<float> Constants{eastl::span<eastl::string>{(eastl::string*)lStrings.begin(), lStrings.size()}};

	for (auto _: state)
	{
		for (const auto& lStr: lStrings)
		{
			Constants[lStr.c_str()] = static_cast<float>(rand() % 100000) / 100000.f;
		}

		// Constants.clear();
	}
}
BENCHMARK(PhmStringTest);

static void UnorderedMap(benchmark::State& state)
{
	eastl::unordered_map<eastl::string_view, float> Constants{};
	eastl::vector<eastl::string>					lStrings = First10000Words();
	Constants.reserve(10000);

	for (auto _: state)
	{
		for (const auto& lStr: lStrings)
		{
			Constants[lStr.c_str()] = static_cast<float>(rand() % 100000) / 100000.f;
		}

		// Constants.clear();
	}
}
BENCHMARK(UnorderedMap);

static void Map(benchmark::State& state)
{
	eastl::map<eastl::string_view, float> Constants{};
	eastl::vector<eastl::string>		  lStrings = First10000Words();
	for (auto _: state)
	{
		for (const auto& lStr: lStrings)
		{
			Constants[lStr.c_str()] = static_cast<float>(rand() % 100000) / 100000.f;
		}

		// Constants.clear();
	}
}
BENCHMARK(Map);

BENCHMARK_MAIN();
