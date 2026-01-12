#include <set>
#include <map>
#include <array>
#include <cmath>

#include <cctype>
#include <limits>
#include <codecvt>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <stdexcept>


#ifdef _WIN32
# include <windows.h>
# include <fcntl.h>
# include <io.h>
#
# undef min
# undef max
#endif

typedef           char  i8;
typedef          short  i16;
typedef            int  i32;

typedef unsigned  char  u8;
typedef unsigned short  u16;
typedef unsigned   int  u32;


// typedef std::map<i16, i32> i16toi16;

using namespace std::string_literals;


struct  cpos : std::array<wchar_t, 2> {};

struct ansi_u8color {
  bool m_f;
  bool m_b;

  u8 m_fr;
  u8 m_fg;
  u8 m_fb;

  u8 m_br;
  u8 m_bg;
  u8 m_bb;

  ansi_u8color()
  : m_f(false),
    m_b(false),
    m_fr(0),
    m_fg(0),
    m_fb(0),
    m_br(0),
    m_bg(0),
    m_bb(0)
  {

  }
  ansi_u8color(
    u8 const &fr,
    u8 const &fg,
    u8 const &fb
  )
  : m_f(true),
    m_b(false),
    m_fr(fr),
    m_fg(fg),
    m_fb(fb),
    m_br(0),
    m_bg(0),
    m_bb(0)
  {

  }
  ansi_u8color(
    u8 const &fr,
    u8 const &fg,
    u8 const &fb,
    u8 const &br,
    u8 const &bg,
    u8 const &bb
  )
  : m_f(true),
    m_b(true),
    m_fr(fr),
    m_fg(fg),
    m_fb(fb),
    m_br(br),
    m_bg(bg),
    m_bb(bb)
  {

  }

  ansi_u8color& reset() {
    this->m_b = false;
    this->m_f = false;

    return *this;
  }

  u16 fg() const {
    return  16 + 
            36*this->m_fr +
             6*this->m_fg +
               this->m_fb;
  };
  ansi_u8color& fg(bool set)
  {
    this->m_f = set;

    return *this;
  }
  ansi_u8color& fg(
    u8 const &fr,
    u8 const &fg,
    u8 const &fb
  )
  {
    this->m_f = true;

    this->m_fr = fr;
    this->m_fg = fg;
    this->m_fb = fb;

    return *this;
  }
  u16 bg() const {
    return  16 + 
            36*this->m_br +
             6*this->m_bg +
               this->m_bb;
  };
  ansi_u8color& bg(bool set)
  {
    this->m_b = set;

    return *this;
  }
  ansi_u8color& bg(
    u8 const &br,
    u8 const &bg,
    u8 const &bb
  )
  {
    this->m_b = true;

    this->m_br = br;
    this->m_bg = bg;
    this->m_bb = bb;

    return *this;
  }

  std::wstring operator()() const {
    using namespace std;

    wstring wstr;
    
    static wstring const esc = L"\033"s;

    if(this->m_b) {
      wstr  = esc + L"[48:5:"s +
              to_wstring(this->bg()) + L'm';
    };

    if(this->m_f) {
      wstr += esc + L"[38:5:"s +
              to_wstring(this->fg()) + L'm';
    };

    if(!this->m_b && !this->m_f) {
      wstr  = esc + L"[0m"s;
    };
    
    return wstr;
  };
  std::wstring operator()(wchar_t const in) const {
    using namespace std;

    constexpr wchar_t const * rst = L"\033[0m";

    wstring out = (*this)() + in + rst;
    
    return out;
  };
  std::wstring operator()(std::wstring const in) const {
    using namespace std;

    constexpr wchar_t const * rst = L"\033[0m";

    wstring out = (*this)() + in + rst;
    
    return out;
  };

  friend std::wostream& operator<<(
    std::wostream& wos,
    ansi_u8color const &color
  ) {

    wos << color();

    return wos;
  }
};

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
    enum struct player {
      _null_,
      one,
      two
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
      addr& operator=(std::string const &str) {
        auto [isValid, msg] = is_valid(str);

        if(!isValid) {
          throw std::runtime_error(msg.c_str());
        }

        char const  alpha =  toupper(isalpha(str[0]) ? str[0] : str[1]),
                    num   =         (isdigit(str[1]) ? str[1] : str[0]);

        this->at(0) = alpha;
        this->at(1) = num;
        
        return *this;
      }
    };

    typedef bool (*pfn_t)(addr const&);
    typedef std::map<   name,  player>  sym_ply_t;
    typedef std::map<   name, wchar_t>  sym_chr_t;
    typedef std::map<   name,   pfn_t>  sym_pfn_t;
    typedef std::map<wchar_t,    name>  chr_sym_t;

    static bool    no_pattern(addr const & _) {
      return false;
    };
    static bool  pawn_pattern(addr const & _) {
      char row = _[1] - '1',
           col = _[0] - 'A';
                    
      bool dst = row == 1 || col == 0;

      // dst &= (row != 0 && col != 0);

      return dst;        
    }
    static bool  rook_pattern(addr const & _) {
      char row = _[1] - '1',
           col = _[0] - 'A';
                    
      bool dst = row == 0 ||
                 col == 0;

      dst &= (row != 0 && col != 0);

      return dst;        
    }
    static bool  knight_pattern(addr const & _) {
      using namespace std;

      char row = _[1] - '1',
           col = _[0] - 'A';
      
      bool dst = (abs(row) == 2 && abs(col) == 1) ||
                 (abs(row) == 1 && abs(col) == 2);

      dst &= (row != 0 && col != 0);

      return dst;
    }
    static bool bishop_pattern(addr const & _) { 
      using namespace std;

      char row = _[1] - '1',
           col = _[0] - 'A';
      
      bool dst = abs(row) == abs(col);

      dst &= (row != 0 && col != 0);

      return dst;
    }

    static bool queen_pattern(addr const & _) { 
      bool rook   =   rook_pattern(_),
           bishop = bishop_pattern(_),
           dst    = rook || bishop;

      return dst;
    };
    static bool  king_pattern(addr const & _) {
      using namespace std;

      char row = _[1] - '1',
           col = _[0] - 'A';
                    
      bool dst = abs(row) < 2 &&
                 abs(col) < 2;

      dst &= (row != 0 && col != 0);

      return dst;        
    }

    static sym_ply_t const e2p;
    static sym_chr_t const e2w;
    static sym_pfn_t const e2f;
    static chr_sym_t const w2e;

    name m_name;
    addr m_pos;
    
    
    piece (
      name const &_name = name::_null_,
      addr const &_pos = "a1"s
    ) : m_name(_name),
        m_pos(_pos)
    {
      // if(this->which_player() == piece::player::one)
      //       this->m_color.fg(3, 1, 0);
      // else  this->m_color.fg(1, 2, 0);
    }
    player which_player() const {
      auto search = piece::e2p.find(this->m_name);

      if(search == piece::e2p.end())
        return piece::player::_null_;

      return search->second;
    }
    pfn_t move_pattern() const {
      typedef std::set<addr> addrs_t;

      // addrs_t dst;

      if( auto search  = piece::e2f.find(this->m_name);
               search != piece::e2f.end()
      ) return search->second;

      return piece::no_pattern;
    }
    friend std::wostream& operator<<(
      std::wostream& wos,
      piece const &pc
    ) {
      static ansi_u8color const c1(5, 3, 0),
                                c2(3, 4, 0);
      
      static std::wstring const _c1  = c1(L"["),
                                 c1_ = c1(L"]"),
                                _c2  = c2(L"["),
                                 c2_ = c2(L"]");

      auto search = piece::e2w.find(pc.m_name);

      if(search == piece::e2w.end())
        return wos;

      // wos << pc.m_color(search != piece::e2w.end() ? search->second : L'?');

      auto const &color = (pc.which_player() == piece::player::one ? c1 : c2);
      wos << color(search != piece::e2w.end() ? search->second : L'?');

      return wos;
    }
  };
  // bool null_fn() { return false; }

  piece::sym_ply_t const piece::e2p = {
    { piece::name::_null_,  piece::player::_null_ },

    { piece::name::wQueen,  piece::player::one },
    { piece::name::wKing,   piece::player::one },
    { piece::name::wRook,   piece::player::one },
    { piece::name::wBishop, piece::player::one },
    { piece::name::wKnight, piece::player::one },
    { piece::name::wPawn,   piece::player::one },

    { piece::name::bQueen,  piece::player::two },
    { piece::name::bKing,   piece::player::two },
    { piece::name::bRook,   piece::player::two },
    { piece::name::bBishop, piece::player::two },
    { piece::name::bKnight, piece::player::two },
    { piece::name::bPawn,   piece::player::two }
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
    { piece::name::bKing,   L'\u2655' },
    { piece::name::bRook,   L'\u2656' },
    { piece::name::bBishop, L'\u2657' },
    { piece::name::bKnight, L'\u2658' },
    { piece::name::bPawn,   L'\u2659' },

    // { piece::name::bQueen,  L'\u2654' },
    // { piece::name::bKing,   L'\u265b' },
    // { piece::name::bRook,   L'\u2656' },
    // { piece::name::bBishop, L'\u265d' },
    // { piece::name::bKnight, L'\u265e' },
    // { piece::name::bPawn,   L'\u265f' }
  };
  piece::sym_pfn_t const piece::e2f = {
    { piece::name::_null_,  piece::no_pattern },

    { piece::name::wQueen,  piece::queen_pattern  },
    { piece::name::wKing,   piece::king_pattern   },
    { piece::name::wRook,   piece::rook_pattern   },
    { piece::name::wBishop, piece::bishop_pattern },
    { piece::name::wKnight, piece::knight_pattern },
    { piece::name::wPawn,   piece::pawn_pattern   },

    { piece::name::bQueen,  piece::queen_pattern  },
    { piece::name::bKing,   piece::king_pattern   },
    { piece::name::bRook,   piece::rook_pattern   },
    { piece::name::bBishop, piece::bishop_pattern },
    { piece::name::bKnight, piece::knight_pattern },
    { piece::name::bPawn,   piece::pawn_pattern   }
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
  struct row : std::array<piece, 8> {
    typedef std::array<piece, 8> base_t;
    static base_t empty() {
      base_t dst;
      dst.fill(piece::name::_null_);
      return dst;
    }
    static base_t ranks(piece::player c) {
      base_t dst;

      bool is_P1 = c == piece::player::one;

      if(is_P1) {
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
    static base_t pawns(piece::player c) {
      base_t dst;
      dst.fill( c == piece::player::one ? 
                  piece::name::wPawn :
                  piece::name::bPawn);
      return dst;
    }


    mutable  bool m_stream_helper,
                  m_highlight_active_pos;

    mutable short m_active_pos;

    // ansi_u8color m_color;

  
    row (
      base_t const &pieces = empty(),
      wchar_t const &separator = L'|'
    ) : base_t(pieces),
        m_stream_helper(false)
    {

    } 

    friend std::wostream& operator<<(
      std::wostream& wos,
      row const &row
    ) {
      constexpr wchar_t const * rst = L"\033[0m";
      // static wchar_t const * rst = L"\033[0m";

      static ansi_u8color const c1(0, 0, 0),
                                c2(1, 1, 1),
                                c3(0, 0, 0, 1, 1, 2),
                                c4(3, 3, 3, 1, 1, 2);
      
      static std::wstring const _c1  = c1(L"["),
                                 c1_ = c1(L"]"),
                                _c2  = c2(L"["),
                                 c2_ = c2(L"]"),
                                _c3  = c3(L"["),
                                 c3_ = c3(L"]"),
                                _c4  = c4(L"["),
                                 c4_ = c4(L"]");
      
      // row.m_color.bg(1, 1, 2);

      bool const &flp = row.m_stream_helper;
      for(std::size_t i = 0; i < row.size(); i++) {
        std::wstringstream ss;
        ss << row[i];
        // wchar_t pc = static_cast<wchar_t>(row[i]);
        
        if(row.m_highlight_active_pos && row.m_active_pos == i) {

            if (i%2 == flp) wos << _c3 << c3(ss.str()) << c3_;
          else              wos << _c4 << c4(ss.str()) << c4_;

        } else {

            if (i%2 == flp) wos << _c1 << row[i] << c1_;
          else              wos << _c2 << row[i] << c2_;

        }
      }

      return wos;
    }
  };
  struct board : std::array<row, 8> {
    typedef std::array<row, 8> base_t;

    static base_t initial_setup() {
      base_t dst;

      const auto one = piece::player::one;
      const auto two = piece::player::two;

      dst[0] = row::ranks(one);
      dst[1] = row::pawns(one);
      dst[2] = row::empty();
      dst[3] = row::empty();
      dst[4] = row::empty();
      dst[5] = row::empty();
      dst[6] = row::pawns(two);
      dst[7] = row::ranks(two);

      return dst;
    }
    
    piece::addr m_active_pos;
    bool m_highlight_active_pos;

    board(base_t const &setup)
    : base_t(setup),
      m_active_pos("a1"),
      m_highlight_active_pos(true)
    { }
    piece::pfn_t active_pattern() const {
      auto  dst = nullptr;
      
      auto  r = static_cast<short>( this->m_active_pos[1] - '1'),
            c = static_cast<short>( this->m_active_pos[0] - 'A');

      auto  p = (*this)[r][c]; // active piece
      
      return p.move_pattern();
    }
    void set_checker_pattern(short row) const {
      this->at(row).m_stream_helper = row % 2;
    }
    void set_highlight_pattern(short row) const {
      auto &r = this->at(row);
      auto &highlight_row = r.m_highlight_active_pos;

      piece::pfn_t highlight_pattern = this->active_pattern();

            highlight_row =             this->m_highlight_active_pos   &&
                    static_cast<short>( this->m_active_pos[1] - '1'  ) == row;

        if( highlight_row )
                       r.m_active_pos = this->m_active_pos[0] - 'A';

    }

    friend std::wostream& operator<<(
      std::wostream& wos,
      board const &brd
    ) {

      // u16 r = from_pos[0] - 'A',
      //     c = from_pos[1] - '1';
      wos << L"   ";
      for(std::size_t i = 0; i < 8; i++) {
        wos << " " << static_cast<char>('A' + i) << " ";
      }
      wos << L"\033[0m\n";

      for(std::size_t i = 0; i < brd.size(); i++) {

        brd.set_checker_pattern(i);
        brd.set_highlight_pattern(i);

       
        wos << L"(" << (i + 1) << L")" << brd[i] << '\n';
      }
      wos << L'\n';

      return wos;
    }
    
  };
  
  enum class command {
    _null_,
      move,
        to,
     clear,
      quit,
  };
  static std::map<std::wstring, command> const cmdEnum = {
    {       L"m"s, command::move },
    {    L"move"s, command::move },
    {      L"to"s, command::to   },
    {   L"clear"s, command::clear   },
    {    L"quit"s, command::quit },
    {       L"q"s, command::quit }
  };
  static std::map<command, std::wstring> const cmdStr = {
    { command::_null_, L"_null_"s,  },
    { command::move, L"move"s,  },
    { command::to,   L"to"s,  },
    { command::quit, L"quit"s,  }
  };
  command resolveCommand(std::wstring const cmd_str) {
    using enum command;

    if( auto search = cmdEnum.find(cmd_str);
        search != cmdEnum.end()
    ) {
      return search->second;
    }
    
    return _null_;
  }
  std::wstring resolveString(command const cmd_enum) {
    using enum command;

    
    if( auto search = cmdStr.find(cmd_enum);
        search != cmdStr.end()
    ) {
      return search->second;
    }
    
    return L""s;
  }
};


int main(int argc, char *argv[]) {
  using  namespace std;
  using  namespace chess;
  using       enum command;

  typedef piece::addr addr;
  typedef piece::player player;

# ifdef _WIN32
  _setmode(_fileno(stdout), _O_U16TEXT);
# endif


  try {
    board game = board::initial_setup();
    wstring token, state_str;

    bool QUIT = false;
    command state = command::_null_;

    while(!QUIT) {
      // wcout << "\033[1;1H\033[2J";
      wcout << game;

      wstring line;
      state_str = resolveString(state);
      wcout << L"P1's turn" << (state_str == L"_null_"s ? L": " : (L" ("s + state_str + L"): "));
      getline(wcin, line);

      wistringstream ss(line);
      // wcout << L"entered '" << ss.str() << "' command.\n";

      while(getline(ss, token, L' ')) {
        // wcout << L"stepping through '" << token << "' token.\n";

        switch(resolveCommand(token)) {
          case move: {
            switch(state) {
              case _null_: {
                state = move;

              } break;
              default:
                wcout << token << L" is an invalid token after 'move'.\n";
              break;
            }
            // wcout << L"move command triggered.\n";
          } break;
          case to: {
            switch(state) {
              case _null_: {
                state = to;
              } break;
              default:
                wcout << token << L" is an invalid token after 'to'.\n";
              break;
            }
          } break;
          case clear: {
            // wcout << L"clear what?\n";
            wcout << "\033[1;1H\033[2J";
          } break;
          case quit: {
            // wcout << L"quit command triggered.\n";
            ask_quit:
              wcout << L"quit? (y/n):\t";
              wchar_t yn;

              wcin >> yn;
              wcin.clear();
              wcin.ignore(numeric_limits<short>::max(), L'\n');

              switch(yn) {
                case L'y':
                  QUIT = true;
                  break;
                case L'n':
                  QUIT = false;
                  break;
                default:
                  wcout << L"\t\tinvalid command.\n";
                  goto ask_quit;
              }
          } break;
          default:
            string narrow_token(token.begin(), token.end());

            switch(state) {
              case move: {
                wcout << L"parsing coordinates\n"; 


                try {
                  piece::addr from_pos(narrow_token);

                  // u16 r = from_pos[0] - 'A',
                  //     c = from_pos[1] - '1';

                  // game[r][c].m_color.bg(1, 1, 2);
                  game.m_active_pos = narrow_token;

                } catch(std::exception &ex) {
                  wcout << L"error parsing move coordinates\n"; 
                } catch(...) {
                  wcout << L"you done fucked up!!!\n"; 
                }

                state = _null_;
              } break;
              case to: {
                wcout << L"parsing coordinates\n"; 

                try {
                  // piece::addr to_pos(narrow_token);
                  // u16 r = to_pos[0] - 'A',
                  //     c = to_pos[1] - '1';

                  game.m_active_pos = narrow_token;
                  game.m_highlight_active_pos = true;

                } catch(std::exception &ex) {
                  wcout << L"error parsing move-to coordinates\n"; 
                }
                
                state = _null_;
              } break;
              default:
                wstring state_str = resolveString(state);
                wcout << token << L" is an invalid token" << (state_str == L"_null_"s ? L"" : (L" after '"s + state_str + L"'")) << ".\n";
              break;
            }
            // wcout << token << L" is an invalid token.\n";
            break;
        }
      }

      // wcout << L"last token parsed: " << token << "\n";
      // line = L"";
    }

  } catch(std::exception &ex) {
    wcout << ex.what();
  }
  
  return 0;
}
