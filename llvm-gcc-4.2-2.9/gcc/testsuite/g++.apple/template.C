/* APPLE LOCAL file templated static data 6298605 */
/* Radar 6298605 */

namespace {
  template<int>
  struct ElfSizes {
    static const int sym_size = 5;
    static const int ehdr_size = 5;
  };

  template<int>
  struct SizedElfFileData {
    SizedElfFileData();

    virtual void ReadSymbols();
  };

  template<int Size>
  SizedElfFileData<Size>::SizedElfFileData() {
    ElfSizes<Size>::ehdr_size;
  }

  template<int Size>
  void SizedElfFileData<Size>::ReadSymbols() {
    ElfSizes<Size>::sym_size;
  }

  void Open() {
    SizedElfFileData<32> foo = SizedElfFileData<32>();
  }
}
