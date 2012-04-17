#include <cpfloat/prop-base/tan.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void tan(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Tan<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
