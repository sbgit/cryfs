#include <google/gtest/gtest.h>
#include <google/gmock/gmock.h>
#include <boost/optional/optional_io.hpp>
#include "../either.h"
#include "../macros.h"

//TODO Go through all test cases and think about whether it makes sense to add the same test case but with primitive types.

using std::string;
using std::vector;
using namespace cpputils;
using ::testing::Test;

class OnlyMoveable {
public:
  OnlyMoveable(int value_): value(value_)  {}
  OnlyMoveable(OnlyMoveable &&source): value(source.value) {source.value = -1;}
  int value;
private:
  DISALLOW_COPY_AND_ASSIGN(OnlyMoveable);
};

template<typename T>
struct StoreWith1ByteFlag {
  T val;
  char flag;
};

class EitherTest: public Test {
public:
  template<class Left, class Right>
  void EXPECT_IS_LEFT(const Either<Left,Right> &val) {
    EXPECT_TRUE(val.is_left());
    EXPECT_FALSE(val.is_right());
  }
  template<class Left, class Right>
  void EXPECT_IS_RIGHT(const Either<Left,Right> &val) {
    EXPECT_FALSE(val.is_left());
    EXPECT_TRUE(val.is_right());
  }
  template<class Left, class Right, class Expected>
  void EXPECT_LEFT_IS(const Expected &expected, Either<Left, Right> &value) {
    EXPECT_IS_LEFT(value);
    EXPECT_EQ(expected, value.left());
    EXPECT_EQ(expected, value.left_opt().get());
    EXPECT_EQ(boost::none, value.right_opt());
    const Either<Left, Right> &const_value = value;
    EXPECT_EQ(expected, const_value.left());
    EXPECT_EQ(expected, const_value.left_opt().get());
    EXPECT_EQ(boost::none, const_value.right_opt());
  }
  template<class Left, class Right, class Expected>
  void EXPECT_RIGHT_IS(const Expected &expected, Either<Left, Right> &value) {
    EXPECT_IS_RIGHT(value);
    EXPECT_EQ(expected, value.right());
    EXPECT_EQ(expected, value.right_opt().get());
    EXPECT_EQ(boost::none, value.left_opt());
    const Either<Left, Right> &const_value = value;
    EXPECT_EQ(expected, const_value.right());
    EXPECT_EQ(expected, const_value.right_opt().get());
    EXPECT_EQ(boost::none, const_value.left_opt());
  }
};

template<typename Left, typename Right>
void TestSpaceUsage() {
  EXPECT_EQ(std::max(sizeof(StoreWith1ByteFlag<Left>), sizeof(StoreWith1ByteFlag<Right>)), sizeof(Either<Left, Right>));
}

TEST_F(EitherTest, SpaceUsage) {
  TestSpaceUsage<char, int>();
  TestSpaceUsage<int, short>();
  TestSpaceUsage<char, short>();
  TestSpaceUsage<int, string>();
  TestSpaceUsage<string, vector<string>>();
}

TEST_F(EitherTest, LeftCanBeConstructed) {
  Either<int, string> val = 3;
  UNUSED(val);
}

TEST_F(EitherTest, RightCanBeConstructed) {
  Either<int, string> val = string("string");
  UNUSED(val);
}

TEST_F(EitherTest, IsLeft) {
  Either<int, string> val = 3;
  EXPECT_IS_LEFT(val);
}

TEST_F(EitherTest, IsRight) {
  Either<int, string> val = string("string");
  EXPECT_IS_RIGHT(val);
}

TEST_F(EitherTest, LeftIsStored) {
  Either<int, string> val = 3;
  EXPECT_LEFT_IS(3, val);
}

TEST_F(EitherTest, RightIsStored) {
  Either<int, string> val = string("string");
  EXPECT_RIGHT_IS("string", val);
}

TEST_F(EitherTest, LeftCanBeMoveContructed) {
  Either<OnlyMoveable, string> val = OnlyMoveable(1);
  UNUSED(val);
}

TEST_F(EitherTest, RightCanBeMoveContructed) {
  Either<string, OnlyMoveable> val = OnlyMoveable(1);
  UNUSED(val);
}

TEST_F(EitherTest, IsLeftWhenMoveContructed) {
  Either<OnlyMoveable, string> val = OnlyMoveable(1);
  EXPECT_IS_LEFT(val);
}

TEST_F(EitherTest, IsRightWhenMoveContructed) {
  Either<string, OnlyMoveable> val = OnlyMoveable(1);
  EXPECT_IS_RIGHT(val);
}

TEST_F(EitherTest, LeftIsStoredWhenMoveContructed) {
  Either<OnlyMoveable, string> val = OnlyMoveable(2);
  EXPECT_EQ(2, val.left().value);
}

TEST_F(EitherTest, RightIsStoredWhenMoveContructed) {
  Either<string, OnlyMoveable> val = OnlyMoveable(3);
  EXPECT_EQ(3, val.right().value);
}

TEST_F(EitherTest, LeftCanBeCopied) {
  Either<string, int> val = string("string");
  Either<string, int> val2 = val;
  EXPECT_LEFT_IS("string", val2);
}

TEST_F(EitherTest, CopyingLeftDoesntChangeSource) {
  Either<string, int> val = string("string");
  Either<string, int> val2 = val;
  EXPECT_LEFT_IS("string", val);
}

TEST_F(EitherTest, RightCanBeCopied) {
  Either<int, string> val = string("string");
  Either<int, string> val2 = val;
  EXPECT_RIGHT_IS("string", val2);
}

TEST_F(EitherTest, CopyingRightDoesntChangeSource) {
  Either<int, string> val = string("string");
  Either<int, string> val2 = val;
  EXPECT_RIGHT_IS("string", val);
}

TEST_F(EitherTest, LeftCanBeMoved) {
  Either<OnlyMoveable, int> val = OnlyMoveable(5);
  Either<OnlyMoveable, int> val2 = std::move(val);
  EXPECT_IS_LEFT(val2);
  EXPECT_EQ(5, val2.left().value);
}

TEST_F(EitherTest, RightCanBeMoved) {
  Either<int, OnlyMoveable> val = OnlyMoveable(5);
  Either<int, OnlyMoveable> val2 = std::move(val);
  EXPECT_IS_RIGHT(val2);
  EXPECT_EQ(5, val2.right().value);
}

TEST_F(EitherTest, ModifyLeft) {
  Either<string, int> val = string("mystring1");
  val.left() = "mystring2";
  EXPECT_LEFT_IS("mystring2", val);
}

TEST_F(EitherTest, ModifyRight) {
  Either<int, string> val = string("mystring1");
  val.right() = "mystring2";
  EXPECT_RIGHT_IS("mystring2", val);
}

TEST_F(EitherTest, ModifyLeftOpt) {
  Either<string, int> val = string("mystring1");
  val.left_opt().get() = "mystring2";
  EXPECT_LEFT_IS("mystring2", val);
}

TEST_F(EitherTest, ModifyRightOpt) {
  Either<int, string> val = string("mystring1");
  val.right_opt().get() = "mystring2";
  EXPECT_RIGHT_IS("mystring2", val);
}

TEST_F(EitherTest, LeftEquals) {
  Either<string, int> val1 = string("mystring");
  Either<string, int> val2 = string("mystring");
  EXPECT_TRUE(val1 == val2);
  EXPECT_TRUE(val2 == val1);
  EXPECT_FALSE(val1 != val2);
  EXPECT_FALSE(val2 != val1);
}

TEST_F(EitherTest, LeftNotEquals) {
  Either<string, int> val1 = string("mystring");
  Either<string, int> val2 = string("mystring2");
  EXPECT_TRUE(val1 != val2);
  EXPECT_TRUE(val2 != val1);
  EXPECT_FALSE(val1 == val2);
  EXPECT_FALSE(val2 == val1);
}

TEST_F(EitherTest, RightEquals) {
  Either<int, string> val1 = string("mystring");
  Either<int, string> val2 = string("mystring");
  EXPECT_TRUE(val1 == val2);
  EXPECT_TRUE(val2 == val1);
  EXPECT_FALSE(val1 != val2);
  EXPECT_FALSE(val2 != val1);
}

TEST_F(EitherTest, RightNotEquals) {
  Either<int, string> val1 = string("mystring");
  Either<int, string> val2 = string("mystring2");
  EXPECT_TRUE(val1 != val2);
  EXPECT_TRUE(val2 != val1);
  EXPECT_FALSE(val1 == val2);
  EXPECT_FALSE(val2 == val1);
}

TEST_F(EitherTest, LeftNotEqualsRight) {
  Either<string, int> val1 = string("mystring");
  Either<string, int> val2 = 3;
  EXPECT_TRUE(val1 != val2);
  EXPECT_TRUE(val2 != val1);
  EXPECT_FALSE(val1 == val2);
  EXPECT_FALSE(val2 == val1);
}

class DestructorCallback {
public:
  MOCK_CONST_METHOD0(call, void());
};
class ClassWithDestructorCallback {
public:
  ClassWithDestructorCallback(const DestructorCallback *destructorCallback) : _destructorCallback(destructorCallback) { }

  ~ClassWithDestructorCallback() {
    _destructorCallback->call();
  }

private:
  const DestructorCallback *_destructorCallback;
};
class OnlyMoveableClassWithDestructorCallback {
public:
  OnlyMoveableClassWithDestructorCallback(const DestructorCallback *destructorCallback) : _destructorCallback(destructorCallback) { }
  OnlyMoveableClassWithDestructorCallback(OnlyMoveableClassWithDestructorCallback &&source): _destructorCallback(source._destructorCallback) {}

  ~OnlyMoveableClassWithDestructorCallback() {
    _destructorCallback->call();
  }

private:
  DISALLOW_COPY_AND_ASSIGN(OnlyMoveableClassWithDestructorCallback);
  const DestructorCallback *_destructorCallback;
};

class EitherTest_Destructor: public EitherTest {
public:
  DestructorCallback destructorCallback;

  void EXPECT_DESTRUCTOR_CALLED(int times = 1) {
    EXPECT_CALL(destructorCallback, call()).Times(times);
  }
};

TEST_F(EitherTest_Destructor, LeftDestructorIsCalled) {
  ClassWithDestructorCallback temp(&destructorCallback);
  Either<ClassWithDestructorCallback, string> var = temp;
  EXPECT_DESTRUCTOR_CALLED(2);  //Once for the temp object, once when the either class destructs
}

TEST_F(EitherTest_Destructor, RightDestructorIsCalled) {
  ClassWithDestructorCallback temp(&destructorCallback);
  Either<string, ClassWithDestructorCallback> var = temp;
  EXPECT_DESTRUCTOR_CALLED(2);  //Once for the temp object, once when the either class destructs
}

TEST_F(EitherTest_Destructor, LeftDestructorIsCalledAfterCopying) {
  ClassWithDestructorCallback temp(&destructorCallback);
  Either<ClassWithDestructorCallback, string> var1 = temp;
  Either<ClassWithDestructorCallback, string> var2 = var1;
  EXPECT_DESTRUCTOR_CALLED(3);  //Once for the temp object, once for var1 and once for var2
}

TEST_F(EitherTest_Destructor, RightDestructorIsCalledAfterCopying) {
  ClassWithDestructorCallback temp(&destructorCallback);
  Either<string, ClassWithDestructorCallback> var1 = temp;
  Either<string, ClassWithDestructorCallback> var2 = var1;
  EXPECT_DESTRUCTOR_CALLED(3);  //Once for the temp object, once for var1 and once for var2
}

TEST_F(EitherTest_Destructor, LeftDestructorIsCalledAfterMoving) {
  OnlyMoveableClassWithDestructorCallback temp(&destructorCallback);
  Either<OnlyMoveableClassWithDestructorCallback, string> var1 = std::move(temp);
  Either<OnlyMoveableClassWithDestructorCallback, string> var2 = std::move(var1);
  EXPECT_DESTRUCTOR_CALLED(3);  //Once for the temp object, once for var1 and once for var2
}

TEST_F(EitherTest_Destructor, RightDestructorIsCalledAfterMoving) {
  OnlyMoveableClassWithDestructorCallback temp(&destructorCallback);
  Either<string, OnlyMoveableClassWithDestructorCallback> var1 = std::move(temp);
  Either<string, OnlyMoveableClassWithDestructorCallback> var2 = std::move(var1);
  EXPECT_DESTRUCTOR_CALLED(3);  //Once for the temp object, once for var1 and once for var2
}