#include <cpfloat/prop-base/cos.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void cos(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Cos<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
