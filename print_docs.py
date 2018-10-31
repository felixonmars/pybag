import pydoc
import pybag.oa
import pybag.schematic
import pybag.util.interval


test_classes = [
    pybag.util.interval.PyDisjointIntervals,
    pybag.schematic.PySchInstRef,
    pybag.schematic.PySchCellView,
    pybag.oa.PyOADatabase,
]

for cls in test_classes:
    strhelp = pydoc.render_doc(cls)
    print(strhelp)
    print()
