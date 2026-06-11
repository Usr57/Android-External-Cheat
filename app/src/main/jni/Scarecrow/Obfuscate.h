#include <cstddef>
#include <string>
#include <array>
#include <utility>

namespace ay
{
    namespace detail {
        template <std::size_t... Is> struct index_sequence {};

        template <std::size_t N, std::size_t... Is>
        struct make_index_sequence_helper : make_index_sequence_helper<N - 1, N - 1, Is...> {};

        template <std::size_t... Is>
        struct make_index_sequence_helper<0, Is...> { using type = index_sequence<Is...>; };

        template <std::size_t N>
        using make_index_sequence = typename make_index_sequence_helper<N>::type;
    }
    template <std::size_t N, char KEY, std::size_t... I>
    constexpr std::array<char, N> obfuscate_array_impl(const char* data, detail::index_sequence<I...>)
    {
        return { (char)(data[I] ^ KEY)... };
    }

    template <std::size_t N, char KEY>
    class obfuscator
    {
    public:
        constexpr obfuscator(const char* data) :
                m_data(obfuscate_array_impl<N, KEY>(data, detail::make_index_sequence<N>{}))
        {
            static_assert(KEY != '\0', "privet XD");
        }

        constexpr const char* getData() const
        {
            return m_data.data();
        }

        constexpr std::size_t getSize() const
        {
            return N;
        }

        constexpr char getKey() const
        {
            return KEY;
        }

    private:
        const std::array<char, N> m_data;
    };


    template <std::size_t N, char KEY>
    class OBFUSCATE_data
    {
    public:
        OBFUSCATE_data(const obfuscator<N, KEY>& obfuscator)
        {
            for (std::size_t i = 0; i < N; i++)
            {
                m_data[i] = obfuscator.getData()[i];
            }
        }

        ~OBFUSCATE_data()
        {
            for (std::size_t i = 0; i < N; i++)
            {
                m_data[i] = 0;
            }
        }

        operator char*()
        {
            decrypt();
            return m_data;
        }

        operator const char*()
        {
            decrypt();
            return m_data;
        }

        operator std::string()
        {
            decrypt();
            return std::string(m_data, N - 1);
        }

        void decrypt()
        {
            if (is_encrypted())
            {
                for (std::size_t i = 0; i < N; i++)
                {
                    m_data[i] ^= KEY;
                }
            }
        }


        void encrypt()
        {
            if (!is_encrypted())
            {
                for (std::size_t i = 0; i < N; i++)
                {
                    m_data[i] ^= KEY;
                }
            }
        }
        bool is_encrypted() const
        {
            return m_data[N - 1] != '\0';
        }

    private:
        char m_data[N]{};
    };


    template <std::size_t N, char KEY = '.'>
    constexpr obfuscator<N, KEY> make_obfuscator(const char(&data)[N])
    {
        return obfuscator<N, KEY>(data);
    }
}

#define OBF(data) OBFUSCATE_KEY(data, '.')
#define OBFUSCATE_KEY(data, key) \
    []() -> ay::OBFUSCATE_data<sizeof(data)/sizeof(data[0]), key>& { \
       constexpr auto n = sizeof(data)/sizeof(data[0]); \
       static_assert(data[n - 1] == '\0', "poka :("); \
       constexpr auto obfuscator = ay::make_obfuscator<n, key>(data); \
       static auto OBFUSCATE_data = ay::OBFUSCATE_data<n, key>(obfuscator); \
       return OBFUSCATE_data; \
    }()