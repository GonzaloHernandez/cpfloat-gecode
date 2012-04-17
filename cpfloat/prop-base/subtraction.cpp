#include <cpfloat/prop-base/subtraction.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void subtraction(Gecode::Space& home, CPFloatVar x, CPFloatVar y, CPFloatVar z) {
    if (home.failed()) return;
    
    CPFloatView left1(x);
    CPFloatView left2(y);
    CPFloatView right(z);
    GECODE_ES_FAIL((Subtraction<CPFloatView,CPFloatView,CPFloatView>::post(home,left1,left2,right)));
  }  
}
