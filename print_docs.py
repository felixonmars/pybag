import pydoc
import pybag.base

test_classes = [
    pybag.base.util.interval.PyDisjointIntervals
]

for cls in test_classes:
    strhelp = pydoc.render_doc(cls)
    print(strhelp)
    print()
