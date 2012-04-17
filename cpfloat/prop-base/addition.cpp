#include <cpfloat/prop-base/addition.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void addition(Gecode::Space& home, CPFloatVar x, CPFloatVar y, CPFloatVar z) {
    if (home.failed()) return;
    
    CPFloatView left1(x);
    CPFloatView left2(y);
    CPFloatView right(z);
    GECODE_ES_FAIL((Addition<CPFloatView,CPFloatView,CPFloatView>::post(home,left1,left2,right)));
  }  
}
