#include <cpfloat/prop-base/asin.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void asin(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Asin<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
