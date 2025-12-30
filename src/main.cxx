#include <map>
#include <array>
#include <cctype>
#include <string_view>

#include <iostream>
#include <iomanip>
#include <stdexcept>

typedef short i16;
typedef   int i32;

typedef std::map<i16, i32> i16toi16;

using namespace std::string_view_literals;

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
      
      constexpr static
      std::pair<bool, std::string_view>
      is_valid(
        std::string_view const &sv
      ) noexcept {
        using namespace std;
        if(sv.length() != 2)
          return pair(false, "position length error.");
        
        if(!isalnum(sv[0]) || !isalnum(sv[1]))
          return pair(false, "argument(s) are not alphanumeric.");

        if( !(isalpha(sv[0]) && isdigit(sv[1])) &&
            !(isalpha(sv[1]) && isdigit(sv[0]))
        ) return pair(  false,
                        "invalid arguments. "
                        "Must be a letter from A through H "
                        "followed a number from 1 through 8"
                        "e.i. \"A1\""); 

        char const  alpha =  toupper(isalpha(sv[0]) ? sv[0] : sv[1]),
                    num   =         (isdigit(sv[1]) ? sv[1] : sv[0]);

        if(alpha >= 'A' && alpha < 'H')
          return pair(false, "Must be a letter from A through H.");

        if(num >= '1' && num < '9')
          return pair(false, "Must be a number from 1 through 8.");
        
        return pair(true, "valid position.");
      }
      
      addr(std::string_view const &sv) {
        auto [isValid, msg_sv] = is_valid(sv);

        if(!isValid) {
          std::string msg(msg_sv.begin(), msg_sv.end());
          throw std::runtime_error(msg.c_str());
        }

        char const  alpha =  toupper(isalpha(sv[0]) ? sv[0] : sv[1]),
                    num   =         (isdigit(sv[1]) ? sv[1] : sv[0]);

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
      addr const &_pos = "__"sv
    ) : m_name(_name),
        m_pos(_pos)
    {

    }

    friend std::wostream& operator<<(
      std::wostream& wos,
      piece const &pc
    ) {
      auto search = piece::e2w.find(pc.m_name);

      if(search == piece::e2w.end())
        return wos;

      wchar_t w = search->second;

      wos << w;

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
    { piece::name::bQueen,  L'\u265a' },
    { piece::name::bKing,   L'\u265b' },
    { piece::name::bRook,   L'\u265c' },
    { piece::name::bBishop, L'\u265d' },
    { piece::name::bKnight, L'\u265e' },
    { piece::name::bPawn,   L'\u265f' },
  };
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
        wos << '[' << row[i] << ']';
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

  board game = board::initial_setup();

  wcout << game;
  
  return 0;
}
