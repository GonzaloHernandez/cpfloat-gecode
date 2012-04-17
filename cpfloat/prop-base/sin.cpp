#include <cpfloat/prop-base/sin.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void sin(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Sin<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
