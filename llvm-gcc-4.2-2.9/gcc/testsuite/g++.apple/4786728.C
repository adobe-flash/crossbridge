/* APPLE LOCAL file 4786728 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-m64 -O1 -g -fstrict-aliasing" } */
typedef __SIZE_TYPE__ size_t;
namespace std
{
  template<typename _Alloc> class allocator;
  template<class _CharT> struct char_traits;
  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> > class basic_string;
  template<> struct char_traits<char>;
  typedef basic_string<char> string;
}

namespace __gnu_cxx
{
  template<typename _Tp> class new_allocator
  {
    public:
      typedef _Tp* pointer;
      new_allocator () throw () { }
      new_allocator (const new_allocator&) throw () { }
      template<typename _Tp1>
        new_allocator (const new_allocator<_Tp1>&) throw () { }
      ~new_allocator () throw () { }
      pointer allocate (size_t __n, const void* = 0)
      { return (pointer)0; }
      void deallocate (pointer __p, size_t) 
      { ::operator delete (__p); }
  };
}

namespace std
{
  template<typename _Tp> class allocator;
  template<typename _Tp>
    class allocator: public __gnu_cxx::new_allocator<_Tp>
    {
      public:
        allocator () throw () { }
        allocator(const allocator& __a) throw ()
          : __gnu_cxx::new_allocator<_Tp> (__a) { }
        template<typename _Tp1>
          allocator (const allocator<_Tp1>&) throw () { }
        ~allocator () throw () { }
    };
}

namespace std
{
  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_string
    {
      private:
        struct _Rep
        {
          static size_t _S_empty_rep_storage[];
          static _Rep& _S_empty_rep ()
            { return *reinterpret_cast<_Rep*>(&_S_empty_rep_storage); }
          _CharT* _M_refdata () throw ()
          { return reinterpret_cast<_CharT*> (this + 1); }
          void _M_dispose (const _Alloc& __a)
          {
            if (__builtin_expect (this != &_S_empty_rep (), false))
              _M_destroy (__a);
          }
          void _M_destroy (const _Alloc&) throw ();
        };
        struct _Alloc_hider : _Alloc
        {
          _Alloc_hider (_CharT* __dat, const _Alloc& __a)
          : _Alloc (__a), _M_p (__dat) { }
          _CharT* _M_p;
        };
        mutable _Alloc_hider _M_dataplus;
        _CharT* _M_data () const
        { return _M_dataplus._M_p; }
        _Rep* _M_rep() const
        { return &((reinterpret_cast<_Rep*> (_M_data ()))[-1]); }
        static _Rep& _S_empty_rep () { return _Rep::_S_empty_rep (); }
      public:
        basic_string ();
        basic_string (const _Alloc& __a);
        basic_string (const basic_string& __str);
        basic_string (const basic_string& __str, size_t __pos, size_t __n);
        basic_string (const basic_string& __str, size_t __pos,
          size_t __n, const _Alloc& __a);
        basic_string (const _CharT* __s, size_t __n,
          const _Alloc& __a = _Alloc());
        basic_string (const _CharT* __s, const _Alloc& __a = _Alloc());
        basic_string (size_t __n, _CharT __c, const _Alloc& __a = _Alloc ());
        template<class _InputIterator>
          basic_string (_InputIterator __beg, _InputIterator __end,
          const _Alloc& __a = _Alloc ());
        ~basic_string ()
        { _M_rep ()->_M_dispose(this->get_allocator ()); }
    private:
      _Alloc get_allocator () const { return _M_dataplus; }
  };

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>::
    basic_string()
    : _M_dataplus (_S_empty_rep()._M_refdata (), _Alloc ()) { }
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+ (const basic_string<_CharT, _Traits, _Alloc>& __lhs,
       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      basic_string<_CharT, _Traits, _Alloc> __str (__lhs);
      return __str;
    }
}

namespace std
{
  template<typename _Tp, typename _Alloc>
    struct _Vector_base
    {
      struct _Vector_impl : public _Alloc
      {
        _Tp* _M_start;
        _Tp* _M_finish;
        _Tp* _M_end_of_storage;
       _Vector_impl (_Alloc const& __a)
       : _Alloc (__a), _M_start (0), _M_finish (0), _M_end_of_storage (0) {}
      };
    public:
      typedef _Alloc allocator_type;
      allocator_type
      get_allocator () const
      { return *static_cast<const _Alloc*> (&this->_M_impl); }
      _Vector_base (const allocator_type& __a) 
      : _M_impl(__a) {}
      _Vector_base(size_t __n, const allocator_type& __a) {}
      ~_Vector_base()
      { _M_deallocate(this->_M_impl._M_start, this->_M_impl._M_end_of_storage
        - this->_M_impl._M_start); }
    public:
      _Vector_impl _M_impl;
      _Tp* _M_allocate (size_t __n) { return _M_impl.allocate (__n); }
      void _M_deallocate (_Tp* __p, size_t __n)
      {
        _M_impl.deallocate (__p, __n);
      }
    };
  template<typename _Tp, typename _Alloc = allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
      typedef _Vector_base<_Tp, _Alloc> _Base;
      typedef vector<_Tp, _Alloc> vector_type;
    public:
      typedef typename _Base::allocator_type allocator_type;
      vector (const allocator_type& __a = allocator_type ())
      : _Base (__a) {}
    };
}

namespace llvm {
  class GlobalVariable;
  class SourceFile;
  class SourceLanguage;
  struct SourceLanguageCache {
    virtual ~SourceLanguageCache () {}
  };
  class SourceFileInfo {
    std::string BaseName;
    const SourceLanguage *Language;
    const GlobalVariable *Descriptor;
    mutable SourceFile *SourceText;
  public:
    SourceFileInfo (const GlobalVariable *Desc, const SourceLanguage &Lang);
    ~SourceFileInfo ();
    SourceFile &getSourceText () const;
  };
}

namespace llvm {
  namespace sys {
    class Path {
      public:
        Path() : path() {}
        Path(const std::string& p) : path(p) {}
        const std::string &toString() const { return path; }
      private:
        mutable std::string path;
    };
  }
}

namespace llvm {
  namespace sys {
    struct MappedFileInfo;
    class MappedFile {
    public:
      MappedFile() : path_(), info_(0) {}
      ~MappedFile() { if (info_) terminate(); }
    private:
      void terminate();
      sys::Path path_;
      mutable MappedFileInfo* info_;
    };
  }
}

namespace llvm {
  class GlobalVariable;
  class SourceFile {
    sys::Path Filename;
    const GlobalVariable *Descriptor;
    sys::MappedFile File;
    mutable std::vector<unsigned> LineOffset;
  public:
    SourceFile(const std::string &fn, const GlobalVariable *Desc)
      : Filename(fn), Descriptor(Desc), File()
    {
      std::string ErrMsg;
      readFile();
    }
    ~SourceFile() {}
  private:
    void readFile();
  };
}

using namespace llvm;
SourceFile &SourceFileInfo::getSourceText() const
{
  sys::Path tmpPath;
  SourceText = new SourceFile(tmpPath.toString(), Descriptor);
  SourceText = new SourceFile(BaseName, Descriptor);
  return *SourceText;
}
