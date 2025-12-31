#include <map>
#include <array>

#include <cctype>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <stdexcept>

#ifdef _WIN32
  #include <windows.h>
  #include <fcntl.h>
  #include <io.h>
#endif

typedef short i16;
typedef   int i32;

typedef std::map<i16, i32> i16toi16;

using namespace std::string_literals;

struct  cpos : std::array<wchar_t, 2> {};
namespace chess {
  struct piece {
    enum struct name {
      _null_,
      wQueen, 
      wKing,
      wRook,
      wBishop,
      wKnight,
      wPawn,
      bQueen, 
      bKing,
      bRook,
      bBishop,
      bKnight,
      bPawn
    };
    enum struct color {
      black,
      white
    };
    struct  addr : std::array<char, 2> {
      typedef std::array<char, 2> base_t;
      
      static
      std::pair<bool, std::string>
      is_valid(
        std::string const &str
      ) noexcept {
        using namespace std;
        typedef pair<bool, string> dst_t;

        stringstream ss;
        char alpha, num;

        if(str.length() != 2) {
          ss  << "length error: string{"
              << str << "} must have exactly 2 characters";
          
          goto failed;
        }
        
        if(!isalnum(str[0]) || !isalnum(str[1])) {
          ss  << "string{"<< str
              << "} argument(s) are not alphanumeric.";

          goto failed;
        }
        if( !(isalpha(str[0]) && isdigit(str[1]) || 
              isalpha(str[1]) && isdigit(str[0])    )
        ) {
          ss  <<  "invalid argument(s): string{" << str
              <<  "} Must have a letter from A through H "
                  "followed a number from 1 through 8 or vice versa"
                  "e.i. \"A1\""; 

          goto failed;
        }

        alpha =  toupper(isalpha(str[0]) ? str[0] : str[1]);
        num   =         (isdigit(str[1]) ? str[1] : str[0]);

        if(!(alpha >= 'A' && alpha <= 'H')) {
          ss  << "Invalid argument. string{"
              << str << "} Must have a letter from A through H.";
        
          goto failed;
        }

        if(!(num >= '1' && num <= '8')) {
          ss  << "Invalid argument. string{"
              << str << "} Must have a number from 1 through 8.";

          goto failed;
        }
        
        passed:
          ss  << "Argument. string{"
              << str << "} Validation completed (passed).";
            return dst_t(true, ss.str());

        failed:
          return dst_t(false, ss.str());
      }
      
      addr(std::string const &str) {
        auto [isValid, msg] = is_valid(str);

        if(!isValid) {
          throw std::runtime_error(msg.c_str());
        }

        char const  alpha =  toupper(isalpha(str[0]) ? str[0] : str[1]),
                    num   =         (isdigit(str[1]) ? str[1] : str[0]);

        this->at(0) = alpha;
        this->at(1) = num;
      }
    };

    typedef std::map<name, wchar_t> sym_chr_t;
    typedef std::map<wchar_t, name> chr_sym_t;

    static sym_chr_t const e2w;
    static chr_sym_t const w2e;

    name m_name;
    addr m_pos;
    piece (
      name const &_name = name::_null_,
      addr const &_pos = "a1"s
    ) : m_name(_name),
        m_pos(_pos)
    {

    }

    friend std::wostream& operator<<(
      std::wostream& wos,
      piece const &pc
    ) {
      auto search = piece::e2w.find(pc.m_name);

      // if(search == piece::e2w.end())
      //   return wos;

      wos << L'[';
      wos << (search != piece::e2w.end() ? search->second : L'?');
      wos << L']';

      return wos;
    }
  };
  piece::sym_chr_t const piece::e2w = {
    { piece::name::_null_,  L' '      },
    { piece::name::wQueen,  L'\u2654' },
    { piece::name::wKing,   L'\u2655' },
    { piece::name::wRook,   L'\u2656' },
    { piece::name::wBishop, L'\u2657' },
    { piece::name::wKnight, L'\u2658' },
    { piece::name::wPawn,   L'\u2659' },

    { piece::name::bQueen,  L'\u2654' },
    { piece::name::bKing,   L'\u265b' },
    { piece::name::bRook,   L'\u2656' },
    { piece::name::bBishop, L'\u265d' },
    { piece::name::bKnight, L'\u265e' },
    { piece::name::bPawn,   L'\u265f' }
  };
  // piece::sym_chr_t const piece::e2w = {
  //   { piece::name::_null_,  L' '      },
  //   { piece::name::wQueen,  L'\u2654' },
  //   { piece::name::wKing,   L'\u2655' },
  //   { piece::name::wRook,   L'\u2656' },
  //   { piece::name::wBishop, L'\u2657' },
  //   { piece::name::wKnight, L'\u2658' },
  //   { piece::name::wPawn,   L'\u2659' },
  //
  //   { piece::name::bQueen,  L'\u265a' },
  //   { piece::name::bKing,   L'\u265b' },
  //   { piece::name::bRook,   L'\u265c' },
  //   { piece::name::bBishop, L'\u265d' },
  //   { piece::name::bKnight, L'\u265e' },
  //   { piece::name::bPawn,   L'\u265f' }
  // };
  piece::chr_sym_t const piece::w2e = {
    { L' '     , piece::name::_null_  },  
    { L'\u2654', piece::name::wQueen  },  
    { L'\u2655', piece::name::wKing   },   
    { L'\u2656', piece::name::wRook   },   
    { L'\u2657', piece::name::wBishop }, 
    { L'\u2658', piece::name::wKnight }, 
    { L'\u2659', piece::name::wPawn   },   
    { L'\u265a', piece::name::bQueen  },  
    { L'\u265b', piece::name::bKing   },   
    { L'\u265c', piece::name::bRook   },   
    { L'\u265d', piece::name::bBishop }, 
    { L'\u265e', piece::name::bKnight }, 
    { L'\u265f', piece::name::bPawn   }
  };

  struct row : std::array<piece, 8> {
    typedef std::array<piece, 8> base_t;
    
    static base_t empty() {
      base_t dst;
      dst.fill(piece::name::_null_);
      return dst;
    }
    static base_t ranks(piece::color c) {
      base_t dst;

      bool is_white = c == piece::color::white;

      if(is_white) {
        dst[0] = piece::name::wRook;
        dst[1] = piece::name::wKnight;
        dst[2] = piece::name::wBishop;
        dst[3] = piece::name::wQueen; 
        dst[4] = piece::name::wKing;
        dst[5] = piece::name::wBishop;
        dst[6] = piece::name::wKnight;
        dst[7] = piece::name::wRook;
      } else {
        dst[0] = piece::name::bRook;
        dst[1] = piece::name::bKnight;
        dst[2] = piece::name::bBishop;
        dst[3] = piece::name::bKing;
        dst[4] = piece::name::bQueen; 
        dst[5] = piece::name::bBishop;
        dst[6] = piece::name::bKnight;
        dst[7] = piece::name::bRook;
      }

      return dst;
    }
    static base_t pawns(piece::color c) {
      base_t dst;
      dst.fill( c == piece::color::white ? 
                  piece::name::wPawn :
                  piece::name::bPawn);
      return dst;
    }

    row (
      base_t const &pieces = empty(),
      wchar_t const &separator = L'|'
    ) : base_t(pieces)
    {

    } 

    friend std::wostream& operator<<(
      std::wostream& wos,
      row const &row
    ) {

      for(std::size_t i = 0; i < row.size(); i++) {
        // wchar_t pc = static_cast<wchar_t>(row[i]);
        wos << row[i];
      }

      return wos;
    }
  };
  struct board : std::array<row, 8> {
    typedef std::array<row, 8> base_t;

    static base_t initial_setup() {
      base_t dst;

      const auto white = piece::color::white;
      const auto black = piece::color::black;

      dst[0] = row::ranks(white);
      dst[1] = row::pawns(white);
      dst[2] = row::empty();
      dst[3] = row::empty();
      dst[4] = row::empty();
      dst[5] = row::empty();
      dst[6] = row::pawns(black);
      dst[7] = row::ranks(black);

      return dst;
    }

    board(base_t const &setup) : base_t(setup) { }

    friend std::wostream& operator<<(
      std::wostream& wos,
      board const &brd
    ) {

      for(std::size_t i = 0; i < brd.size(); i++) {
        wos << brd[i] << '\n';
      }
      wos << '\n';

      return wos;
    }
    
  };

};




int main(int argc, char *argv[]) {
  using namespace std;
  using namespace chess;

  typedef piece::addr addr;
  typedef piece::color color;

# ifdef _WIN32
  _setmode(_fileno(stdout), _O_U16TEXT);
# endif


  try {
    board game = board::initial_setup();
    wcout << game;

    // row r = row::ranks(color::white);
    // wcout << r; 

    // piece pc(piece::name::wKing);
    // wcout << pc;
    //

    //
    // cout << addr::is_valid("A1").second << "\n";
    // cout << addr::is_valid("H4").second << "\n";
    // cout << addr::is_valid("H8").second << "\n";
    // cout << addr::is_valid("5B").second << "\n";
    // cout << addr::is_valid("XX").second << "\n";
    // cout << addr::is_valid("__").second << "\n";

  } catch(std::exception &ex) {
    wcout << ex.what();
  }
  
  return 0;
}
