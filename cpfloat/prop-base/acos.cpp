#include <cpfloat/prop-base/acos.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void acos(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Acos<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
