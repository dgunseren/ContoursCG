#include "../src/task.h"  // Include your Contour class
#include <gtest/gtest.h>

// ✅ Test if adding segments maintains validity
TEST(ContourTest, IsValidTest) {
    Contour c;
    c.addLineSegment({0, 0}, {10, 10});
    c.addLineSegment({10, 10}, {20, 20});
    EXPECT_TRUE(c.isValid(0.1));
}

// ✅ Test invalid contour (non-connected segments)
TEST(ContourTest, InvalidContour) {
    Contour c;
    c.addLineSegment({0, 0}, {10, 10});
    c.addLineSegment({20, 20}, {30, 30});  // Gap between points
    EXPECT_FALSE(c.isValid(0.1));
}
TEST(ContourTest,RemoveSegmentTest) {
    Contour c;
    c.addLineSegment({11,12},{13,14});
    c.addLineSegment({13,14},{17,18});
    c.addArc({17,18},{19,20},5,5,true);
    c.addArc({170,18},{19,20},5,5,true);
    c.addSegmentAtIndex(2,LineSegment{{17,18},{170,18}});
    c.removeSegmentAtIndex(3);
    EXPECT_TRUE(c.isValid(0.1));
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();  // ✅ Ensures tests run
}

