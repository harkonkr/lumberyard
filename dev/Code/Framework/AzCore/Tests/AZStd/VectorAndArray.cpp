/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#include "UserTypes.h"
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/containers/fixed_vector.h>
#include <AzCore/std/containers/array.h>
#include <AzCore/std/containers/bitset.h>

#include <AzCore/std/allocator_static.h>
#include <AzCore/std/allocator_ref.h>

#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <AzCore/std/utils.h>

using namespace AZStd;
using namespace UnitTestInternal;

/**
 * Make sure a vector is empty, and control all functions to return the proper values.
 * Empty vector as all AZStd containers should not have allocated any memory. Empty and clean containers are not the same.
 */
#define AZ_TEST_VALIDATE_EMPTY_VECTOR(_Vector)        \
    EXPECT_TRUE(_Vector.validate());               \
    EXPECT_EQ(0, _Vector.size());              \
    EXPECT_TRUE(_Vector.empty());                  \
    EXPECT_EQ(0, _Vector.capacity());          \
    EXPECT_TRUE(_Vector.begin() == _Vector.end()); \
    EXPECT_EQ(nullptr, _Vector.data())

/**
 * Validate a vector for certain number of elements.
 */
#define AZ_TEST_VALIDATE_VECTOR(_Vector, _NumElements)                                                        \
    EXPECT_TRUE(_Vector.validate());                                                                       \
    EXPECT_EQ(_NumElements, _Vector.size());                                                           \
    EXPECT_TRUE((_NumElements > 0) ? !_Vector.empty() : _Vector.empty());                                  \
    EXPECT_TRUE((_NumElements > 0) ? _Vector.capacity() >= _NumElements : true);                           \
    EXPECT_TRUE((_NumElements > 0) ? _Vector.begin() != _Vector.end() : _Vector.begin() == _Vector.end()); \
    EXPECT_NE(nullptr, _Vector.data())

namespace UnitTest
{
    struct MyCtorClass
    {
        MyCtorClass() { ++s_numConstructedObjects; }
        ~MyCtorClass() { --s_numConstructedObjects; }

        static int s_numConstructedObjects;
    };

    int MyCtorClass::s_numConstructedObjects = 0;

    struct VectorMoveOnly
    {
        VectorMoveOnly() = default;
        VectorMoveOnly(int num)
            : m_num(num)
        {
        }
        VectorMoveOnly(const VectorMoveOnly&) = delete;
        VectorMoveOnly& operator=(const VectorMoveOnly&) = delete;

        VectorMoveOnly(VectorMoveOnly&& other)
            : m_num(other.m_num)
        {
            other.m_num = 0;
        }
        VectorMoveOnly& operator=(VectorMoveOnly&& other)
        {
            m_num = other.m_num;
            other.m_num = 0;
            return *this;
        }

        int m_num = 0;
    };

    class Arrays
        : public AllocatorsFixture
    {

        void SetUp() override
        {
            AllocatorsFixture::SetUp();
            MyCtorClass::s_numConstructedObjects = 0;
        }
    };

    TEST_F(Arrays, Pair)
    {
        int val1 = 20;
        int val2 = 30;
        AZStd::pair<int, int> pi1(val1, val2);
        AZStd::pair<int, int> pi2(val2, val1);
        AZStd::pair<int&, int&> pr1(val1, val2);
        AZStd::pair<int&, int&> pr2(val2, val1);

        AZ_TEST_ASSERT(pi1 == pi1);
        AZ_TEST_ASSERT(pr1 == pr1);
        AZ_TEST_ASSERT(pi1 == pr1);

        AZ_TEST_ASSERT(pi1 != pi2);
        AZ_TEST_ASSERT(pr1 != pr2);
        AZ_TEST_ASSERT(pi1 != pr2);

        AZ_TEST_ASSERT(pi1 <= pi2);
        AZ_TEST_ASSERT(pr1 <= pr2);
        AZ_TEST_ASSERT(pi1 <= pr2);

        AZ_TEST_ASSERT(pi1 < pi2);
        AZ_TEST_ASSERT(pr1 < pr2);
        AZ_TEST_ASSERT(pi1 < pr2);

        AZ_TEST_ASSERT(pi2 >= pi1);
        AZ_TEST_ASSERT(pr2 >= pr1);
        AZ_TEST_ASSERT(pi2 >= pr1);

        AZ_TEST_ASSERT(pi2 > pi1);
        AZ_TEST_ASSERT(pr2 > pr1);
        AZ_TEST_ASSERT(pi2 > pr1);
    }

    TEST_F(Arrays, PairConstructSucceeds)
    {
        struct FirstElement
        {
            FirstElement() = default;
            FirstElement(int32_t value)
                : m_value(value)
            {
            }

            int32_t m_value{};
        };

        struct SecondElement
        {
            SecondElement() = default;
            SecondElement(double value, bool selected)
                : m_value{ value }
                , m_selected{ selected }
            {
            }

            double m_value{};
            bool m_selected{};
        };
        
        AZStd::pair<FirstElement, SecondElement> testPair(AZStd::piecewise_construct_t{}, AZStd::forward_as_tuple(42), AZStd::forward_as_tuple(16.0, true));
        EXPECT_EQ(42, testPair.first.m_value);
        EXPECT_DOUBLE_EQ(16.0, testPair.second.m_value);
        EXPECT_TRUE(testPair.second.m_selected);
    }

    TEST_F(Arrays, Vector)
    {
        // VectorContainerTest-Begin

        typedef vector<int> vector_int_type;
        //////////////////////////////////////////////////////////////////////////////////////////
        // Vector functionality

        // Default vector (integral type).
        vector_int_type int_vector_default;
        AZ_TEST_VALIDATE_EMPTY_VECTOR(int_vector_default);

        // Default vector (non-integral type).
        vector<MyClass> myclass_vector_default;
        AZ_TEST_VALIDATE_EMPTY_VECTOR(myclass_vector_default);

        // Create a vector (using fill ctor, with memset optimization to set the values)
        vector<char> char_vector(10, 'A');
        AZ_TEST_VALIDATE_VECTOR(char_vector, 10);
        for (vector<char>::iterator iter = char_vector.begin(); iter != char_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(*iter == 'A');
        }

        // Fill ctor with out memset optimization. validate iterators too.
        vector_int_type int_vector(33, 55);
        AZ_TEST_VALIDATE_VECTOR(int_vector, 33);
        for (vector_int_type::iterator iter = int_vector.begin(); iter != int_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(int_vector.validate_iterator(iter));
            AZ_TEST_ASSERT(*iter == 55);
        }
        AZ_TEST_ASSERT(int_vector.validate_iterator(int_vector.end()) == isf_valid);

        // Fill ctor non-intergral type
        vector<MyClass> myclass_vector(22, MyClass(11));
        AZ_TEST_VALIDATE_VECTOR(myclass_vector, 22);
        for (vector<MyClass>::iterator iter = myclass_vector.begin(); iter != myclass_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(iter->m_data == 11);
        }

        // Iter copy ctor
        vector_int_type int_vector2(int_vector.begin(), int_vector.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector2, 33);
        AZ_TEST_ASSERT(int_vector2 == int_vector);
        AZ_TEST_ASSERT(int_vector2 != int_vector_default);

        // Copy ctor.
        vector_int_type int_vector1(int_vector);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 33);
        AZ_TEST_ASSERT(int_vector1 == int_vector);
        AZ_TEST_ASSERT(int_vector1 != int_vector_default);

        // reserve
        int_vector1.reserve(200);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 33);
        AZ_TEST_ASSERT(int_vector1.capacity() == 200);

        // resize with default value
        int_vector1.resize(int_vector1.size() + 1);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 34);
        AZ_TEST_ASSERT(int_vector1.front() == 55);
        AZ_TEST_ASSERT(int_vector1.back() == 0);  // default value

        // resize with provided value
        int_vector1.resize(int_vector1.size() + 1, 60);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 35);
        AZ_TEST_ASSERT(int_vector1.back() == 60);

        // use the we have 3 different values and check the access operators
        for (AZStd::size_t i = 0; i < int_vector1.size(); ++i)
        {
            AZ_TEST_ASSERT(int_vector1[i] == int_vector1.at(i));
        }

        // resize with trim
        int_vector1.resize(10);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);
        AZ_TEST_ASSERT(int_vector1.front() == 55);
        AZ_TEST_ASSERT(int_vector1.back() == 55);

        // push back
        int_vector1.push_back();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 11);

        // pop back
        int_vector1.pop_back();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);

        // push_back with value.
        int_vector1.push_back(100);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 11);
        AZ_TEST_ASSERT(int_vector1.back() == 100);

        // set capacity
        int_vector1.set_capacity(11);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 11);
        AZ_TEST_ASSERT(int_vector1.capacity() == 11);
        AZ_TEST_ASSERT(int_vector1.back() == 100);

        // push back with capacity and change capacity!
        int_vector1.push_back();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 12);
        AZ_TEST_ASSERT(int_vector1.capacity() >= 12);

        int_vector1.set_capacity(11);
        int_vector1.push_back(101);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 12);
        AZ_TEST_ASSERT(int_vector1.capacity() >= 12);
        AZ_TEST_ASSERT(int_vector1.back() == 101);

        // push back with capacity and change capacity!
        int_vector1.set_capacity(11);
        int_vector1.push_back(101);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 12);
        AZ_TEST_ASSERT(int_vector1.capacity() >= 12);
        AZ_TEST_ASSERT(int_vector1.back() == 101);

        // insert

        // insert at the end with capacity change
        int_vector1.set_capacity(12);
        int_vector1.insert(int_vector1.end(), 201);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 13);
        AZ_TEST_ASSERT(int_vector1.capacity() >= 13);
        AZ_TEST_ASSERT(int_vector1.back() == 201);

        // insert without capacity change
        int_vector1.insert(int_vector1.end() - 1, 5, 202);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 18);
        AZ_TEST_ASSERT(int_vector1.back() == 201);
        AZ_TEST_ASSERT(*(int_vector1.end() - 2) == 202);

        // insert with overlapping areas
        int_vector1.insert(int_vector1.end() - 2, 1, 203);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 19);
        AZ_TEST_ASSERT(int_vector1.back() == 201);
        AZ_TEST_ASSERT(*(int_vector1.end() - 3) == 203);

        // insert from another vector.
        int_vector1.insert(int_vector1.end(), int_vector.begin(), int_vector.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 19 + int_vector.size());
        AZ_TEST_ASSERT(int_vector1.back() == 55); // last element from int_vector.

        // insert with initializer list.
        int_vector1.insert(int_vector1.end(), { 23, 24, 25 });
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 55);
        AZ_TEST_ASSERT(int_vector1.back() == 25);

        // erase
        int_vector1.erase(int_vector1.begin(), int_vector1.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 0); // Zero elements but valid capacity.

        int_vector1.push_back(10);
        int_vector1.push_back(20);
        int_vector1.push_back(30);
        vector_int_type::iterator iter = int_vector1.erase(int_vector1.begin() + 1);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 2);
        AZ_TEST_ASSERT(*iter == 30);
        AZ_TEST_ASSERT(int_vector1.front() == 10);

        // clear
        int_vector1.clear();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 0); // Zero elements but valid capacity.

        // swap
        int_vector1.swap(int_vector);
        AZ_TEST_VALIDATE_VECTOR(int_vector, 0);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 33);
        AZ_TEST_ASSERT(int_vector1.front() == 55);

        // swap rvalue reference binding to temporary
        int_vector1.swap(vector_int_type());
        AZ_TEST_VALIDATE_EMPTY_VECTOR(int_vector1);


        // assign
        int_vector1.assign(10, 15);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);

        int_vector.assign(int_vector1.begin(), int_vector1.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector, 10);
        AZ_TEST_ASSERT(int_vector.front() == 15);

        // alignment

        // default int alignment
        AZ_TEST_ASSERT(((AZStd::size_t)int_vector.data() % 4) == 0); // default int alignment

        // make sure every vector allocation is aligned.
        vector<MyClass> aligned_vector(5, 99);
        AZ_TEST_ASSERT(((AZStd::size_t)aligned_vector.data() & (alignment_of<MyClass>::value - 1)) == 0);
        AZ_TEST_ASSERT(((AZStd::size_t)&aligned_vector[0] & (alignment_of<MyClass>::value - 1)) == 0);

        // reverse iterators
        int_vector.clear();
        int_vector.push_back(1);
        int_vector.push_back(2);
        int_vector.push_back(3);
        int_vector.push_back(4);
        int i = 4;
        for (auto it = int_vector.rbegin(); it != int_vector.rend(); ++it, --i)
        {
            AZ_TEST_ASSERT(int_vector.validate_iterator(it));
            AZ_TEST_ASSERT(*it == i);
        }
        i = 4;
        for (auto it = int_vector.rbegin(); it != int_vector.rend(); ++it, --i)
        {
            AZ_TEST_ASSERT(int_vector.validate_iterator(it));
            AZ_TEST_ASSERT(*it == i);
        }

        // resize_no_construct test (it technically behaves like reserve that updates the size of the container)
        vector<MyCtorClass> myCtorClassArray;
        AZ_TEST_ASSERT(MyCtorClass::s_numConstructedObjects == 0);
        myCtorClassArray.resize_no_construct(10);
        AZ_TEST_ASSERT(myCtorClassArray.size() == 10);
        // Test that no constructors have been called even though we resized the container to 10 elements
        AZ_TEST_ASSERT(MyCtorClass::s_numConstructedObjects == 0);

        // test that resize_no_construct shrinks the container if eneded same as resize
        myCtorClassArray.resize_no_construct(0);
        AZ_TEST_ASSERT(myCtorClassArray.empty());
        AZ_TEST_ASSERT(MyCtorClass::s_numConstructedObjects == -10); // we should destroy all objects that we did not construct

        // Vector functionality
        //////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////////
        // Vector allocator tests
        typedef static_buffer_allocator<16*1024, 0> staic_buffer_16KB;
        staic_buffer_16KB myMemoryManager1;
        staic_buffer_16KB myMemoryManager2;
        typedef allocator_ref<staic_buffer_16KB> static_allocator_ref_type;
        static_allocator_ref_type allocator1(myMemoryManager1, "Mystack allocator 1");
        static_allocator_ref_type allocator2(myMemoryManager2, "Mystack allocator 2");

        typedef vector<int, static_allocator_ref_type > IntVectorMyAllocator;
        IntVectorMyAllocator int_vector10(100, 13, allocator1); /// Allocate 100 elements using memory manager 1
        AZ_TEST_VALIDATE_VECTOR(int_vector10, 100);
        AZ_TEST_ASSERT(myMemoryManager1.get_allocated_size() == 100 * sizeof(int));

        // leak_and_reset
        int_vector10.leak_and_reset();  /// leave the allocated memory and reset the vector.
        AZ_TEST_VALIDATE_EMPTY_VECTOR(int_vector10);
        AZ_TEST_ASSERT(myMemoryManager1.get_allocated_size() == 100 * sizeof(int));
        myMemoryManager1.reset(); /// discard the memory

        // allocate again from myMemoryManager1
        int_vector10.resize(100, 15);

        int_vector10.set_allocator(allocator2);
        AZ_TEST_VALIDATE_VECTOR(int_vector10, 100);
        // now we move the allocated size from menager1 to manager2 (without freeing menager1)
        AZ_TEST_ASSERT(myMemoryManager1.get_allocated_size() == myMemoryManager2.get_allocated_size());

        myMemoryManager1.reset(); // flush manager 1 again (int_vector10 is stored in manager 2)

        // swap with different allocators
        IntVectorMyAllocator int_vector11(50, 25, allocator1); // create copy in manager1
        AZ_TEST_VALIDATE_VECTOR(int_vector11, 50);

        int_vector11.swap(int_vector10); // swap the vectors content (since the allocators are different)
        AZ_TEST_VALIDATE_VECTOR(int_vector10, 50);
        AZ_TEST_VALIDATE_VECTOR(int_vector11, 100);
        AZ_TEST_ASSERT(int_vector11.front() == 15);
        AZ_TEST_ASSERT(int_vector10.front() == 25);

        //////////////////////////////////////////////////////////////////////////////////////////
        // Test asserts (which don't cause throw exceptions)
        int_vector10.clear();
        AZ_TEST_START_TRACE_SUPPRESSION;
        int_vector10.reserve(1000000);  // too many elements, 1 assert on too many, 1 assert on allocator returning NULL
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

#ifdef AZSTD_HAS_CHECKED_ITERATORS
        int_vector.clear();
        iter = int_vector.end();
        AZ_TEST_START_TRACE_SUPPRESSION;
        int b = *iter; // the end if is valid but can not dereferenced
        (void)b;
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

        int_vector.push_back(1);
        AZ_TEST_START_TRACE_SUPPRESSION;
        int_vector.validate_iterator(iter); // The push back should make the end iterator invalid.
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

        iter = int_vector.begin();
        int_vector.clear();
        AZ_TEST_START_TRACE_SUPPRESSION;
        int_vector.validate_iterator(iter); // The clear should invalidate all iterators
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);
#endif
        //////////////////////////////////////////////////////////////////////////////////////////
        // Vector rvalue refs test
        int_vector.clear();
        int_vector.resize(33, 55);

        void* data = int_vector.data();
        int_vector1 = AZStd::move(int_vector);
        AZ_TEST_ASSERT(int_vector1.data() == data);

        AZStd::vector<int> int_moved_vector(AZStd::move(int_vector1));
        AZ_TEST_ASSERT(int_moved_vector.data() == data);

        myclass_vector.clear();
        myclass_vector.push_back(MyClass(23));
        AZ_TEST_ASSERT(myclass_vector.size() == 1);
        AZ_TEST_ASSERT(myclass_vector[0].m_data == 23);
        AZ_TEST_ASSERT(myclass_vector[0].m_isMoved == true); // the compiler should move the class automatically

        myclass_vector.emplace_back(44);
        AZ_TEST_ASSERT(myclass_vector.size() == 2);
        AZ_TEST_ASSERT(myclass_vector[1].m_data == 44);
        AZ_TEST_ASSERT(myclass_vector[1].m_isMoved == false);

        myclass_vector.emplace(myclass_vector.begin(), 33);
        AZ_TEST_ASSERT(myclass_vector.size() == 3);
        AZ_TEST_ASSERT(myclass_vector[0].m_data == 33);
        AZ_TEST_ASSERT(myclass_vector[0].m_isMoved == false);

        myclass_vector.insert(myclass_vector.begin() + 1, 22);
        AZ_TEST_ASSERT(myclass_vector.size() == 4);
        AZ_TEST_ASSERT(myclass_vector[0].m_data == 33);
        AZ_TEST_ASSERT(myclass_vector[1].m_data == 22);
        AZ_TEST_ASSERT(myclass_vector[1].m_isMoved == true);
        AZ_TEST_ASSERT(myclass_vector[2].m_data == 23);

        // move iterator
        AZStd::vector<VectorMoveOnly> move_only_vector;
        move_only_vector.push_back(1);
        move_only_vector.push_back(2);
        move_only_vector.push_back(3);
        move_only_vector.push_back(4);

        AZStd::vector<VectorMoveOnly> result_move_only_vector{ AZStd::make_move_iterator(move_only_vector.begin()), AZStd::make_move_iterator(move_only_vector.end()) };
        
        for (const auto& move_only1 : move_only_vector)
        {
            EXPECT_EQ(0, move_only1.m_num);
        }

        int uniquePtrIntValue = 1;
        for (const auto& result_move_only : result_move_only_vector)
        {
            EXPECT_EQ(uniquePtrIntValue, result_move_only.m_num);
            ++uniquePtrIntValue;
        }
        // VectorContainerTest-End
    }


    TEST_F(Arrays, FixedVector)
    {
        // FixedVectorContainerTest-Begin

        //////////////////////////////////////////////////////////////////////////////////////////
        // Fixed Vector functionality

        // Default vector (integral type).
        fixed_vector<int, 50> int_vector_default;
        AZ_TEST_VALIDATE_VECTOR(int_vector_default, 0);

        // Default vector (non-integral type).
        fixed_vector<MyClass, 10> myclass_vector_default;
        AZ_TEST_VALIDATE_VECTOR(myclass_vector_default, 0);

        // Create a vector (using fill ctor, with memset optimization to set the values)
        typedef fixed_vector<char, 10> char_10_type;
        char_10_type char_vector(10, 'A');
        AZ_TEST_VALIDATE_VECTOR(char_vector, 10);
        for (char_10_type::iterator iter = char_vector.begin(); iter != char_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(*iter == 'A');
        }

        // Fill ctor with out memset optimization. validate iterators too.
        typedef fixed_vector<int, 50> int_50_t;
        int_50_t int_vector(33, 55);
        AZ_TEST_VALIDATE_VECTOR(int_vector, 33);
        for (int_50_t::iterator iter = int_vector.begin(); iter != int_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(int_vector.validate_iterator(iter));
            AZ_TEST_ASSERT(*iter == 55);
        }
        AZ_TEST_ASSERT(int_vector.validate_iterator(int_vector.end()) == isf_valid);

        // Fill ctor non-intergral type
        typedef fixed_vector<MyClass, 22> myclass_22_t;
        myclass_22_t myclass_vector(22, MyClass(11));
        AZ_TEST_VALIDATE_VECTOR(myclass_vector, 22);
        for (myclass_22_t::iterator iter = myclass_vector.begin(); iter != myclass_vector.end(); ++iter)
        {
            AZ_TEST_ASSERT(iter->m_data == 11);
        }

        // Iter copy ctor
        int_50_t int_vector2(int_vector.begin(), int_vector.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector2, 33);
        AZ_TEST_ASSERT(int_vector2 == int_vector);
        //AZ_TEST_ASSERT(int_vector2!=int_vector_default);

        // Copy ctor.
        int_50_t int_vector1(int_vector);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 33);
        AZ_TEST_ASSERT(int_vector1 == int_vector);
        //AZ_TEST_ASSERT(int_vector1!=int_vector_default);

        // resize with default value
        int_vector1.resize(int_vector1.size() + 1);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 34);
        AZ_TEST_ASSERT(int_vector1.front() == 55);
        AZ_TEST_ASSERT(int_vector1.back() == 0);

        // resize with provided value
        int_vector1.resize(int_vector1.size() + 1, 60);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 35);
        AZ_TEST_ASSERT(int_vector1.back() == 60);

        // use the we have 3 different values and check the access operators
        for (AZStd::size_t i = 0; i < int_vector1.size(); ++i)
        {
            AZ_TEST_ASSERT(int_vector1[i] == int_vector1.at(i));
        }

        // resize with trim
        int_vector1.resize(10);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);
        AZ_TEST_ASSERT(int_vector1.front() == 55);
        AZ_TEST_ASSERT(int_vector1.back() == 55);

        // push back
        int_vector1.push_back();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 11);

        // pop back
        int_vector1.pop_back();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);

        // push_back with value.
        int_vector1.push_back(100);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 11);
        AZ_TEST_ASSERT(int_vector1.back() == 100);

        //// insert

        // insert at the end
        int_vector1.insert(int_vector1.end(), 201);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 12);
        AZ_TEST_ASSERT(int_vector1.back() == 201);

        // insert without capacity change
        int_vector1.insert(int_vector1.end() - 1, 5, 202);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 17);
        AZ_TEST_ASSERT(int_vector1.back() == 201);
        AZ_TEST_ASSERT(*(int_vector1.end() - 2) == 202);

        // insert with overlapping areas
        int_vector1.insert(int_vector1.end() - 2, 1, 203);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 18);
        AZ_TEST_ASSERT(int_vector1.back() == 201);
        AZ_TEST_ASSERT(*(int_vector1.end() - 3) == 203);

        // insert from another vector.
        int_vector1.insert(int_vector1.end(), int_vector.begin(), int_vector.begin() + 3);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 21);
        AZ_TEST_ASSERT(int_vector1.back() == 55); // last element from int_vector.

        // erase
        int_vector1.erase(int_vector1.begin(), int_vector1.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 0);

        int_vector1.push_back(10);
        int_vector1.push_back(20);
        int_vector1.push_back(30);
        int_50_t::iterator iter = int_vector1.erase(int_vector1.begin() + 1);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 2);
        AZ_TEST_ASSERT(*iter == 30);
        AZ_TEST_ASSERT(int_vector1.front() == 10);

        // clear
        int_vector1.clear();
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 0);

        // swap
        int_vector1.swap(int_vector);
        AZ_TEST_VALIDATE_VECTOR(int_vector, 0);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 33);
        AZ_TEST_ASSERT(int_vector1.front() == 55);

        // assign
        int_vector1.assign(10, 15);
        AZ_TEST_VALIDATE_VECTOR(int_vector1, 10);

        int_vector.assign(int_vector1.begin(), int_vector1.end());
        AZ_TEST_VALIDATE_VECTOR(int_vector, 10);
        AZ_TEST_ASSERT(int_vector.front() == 15);

        // alignment

        // default int alignment
        AZ_TEST_ASSERT(((AZStd::size_t)int_vector.data() % 4) == 0); // default int alignment

        // make sure every vector allocation is aligned. My class is aligned on 32 bytes.
        myclass_vector_default.push_back(MyClass(10));
        AZ_TEST_ASSERT(((AZStd::size_t)myclass_vector_default.data() & (alignment_of<MyClass>::value - 1)) == 0);
        AZ_TEST_ASSERT(((AZStd::size_t)&myclass_vector_default[0] & (alignment_of<MyClass>::value - 1)) == 0);

        // reverse iterators

        // Fixed Vector functionality
        //////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////////
        // Test asserts (which don't cause throw exceptions)
#ifdef AZSTD_HAS_CHECKED_ITERATORS
        int_vector.clear();
        iter = int_vector.end();
        AZ_TEST_START_TRACE_SUPPRESSION;
        int b = *iter;  // the end if is valid but can not dereferenced
        (void)b;
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

        int_vector.push_back(1);
        AZ_TEST_START_TRACE_SUPPRESSION;
        int_vector.validate_iterator(iter);  // The push back should make the end iterator invalid.
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);

        iter = int_vector.begin();
        int_vector.clear();
        AZ_TEST_START_TRACE_SUPPRESSION;
        int_vector.validate_iterator(iter);  // The clear should invalidate all iterators
        AZ_TEST_STOP_TRACE_SUPPRESSION(1);
#endif
        // FixedVectorContainerTest-End
    }

    TEST_F(Arrays, FixedVectorSwapSucceeds)
    {
        // Test dealing with fixed_vectors with big sizes.
        // Have to heap allocated since they wont fit in the stack
        constexpr int bigFixedVectorSize = 10000000; // enough to make it fail without the fix
        AZStd::unique_ptr<fixed_vector<char, bigFixedVectorSize>> big_fixed_vector0 = AZStd::make_unique<fixed_vector<char, bigFixedVectorSize>>();
        AZStd::unique_ptr<fixed_vector<char, bigFixedVectorSize>> big_fixed_vector1 = AZStd::make_unique<fixed_vector<char, bigFixedVectorSize>>();

        big_fixed_vector0->insert(big_fixed_vector0->end(), bigFixedVectorSize, 0);
        big_fixed_vector1->insert(big_fixed_vector1->end(), bigFixedVectorSize, 1);

        EXPECT_EQ(big_fixed_vector0->at(0), 0);
        EXPECT_EQ(big_fixed_vector1->at(0), 1);

        // test swap
        big_fixed_vector0->swap(*big_fixed_vector1);

        EXPECT_EQ(big_fixed_vector0->at(0), 1);
        EXPECT_EQ(big_fixed_vector1->at(0), 0);
    }


    TEST_F(Arrays, VectorSwap)
    {
        vector<void*> vec1(42, nullptr);
        vector<void*> vec2(3, reinterpret_cast<void*>((intptr_t)0xdeadbeef));
        vector<void*> vec3(3, reinterpret_cast<void*>((intptr_t)0xcdcdcdcd));

        vec1.swap(vec2);
        EXPECT_EQ(3, vec1.size());
        EXPECT_EQ(reinterpret_cast<void*>((intptr_t)0xdeadbeef), vec1[0]);
        EXPECT_EQ(42, vec2.size());
        EXPECT_EQ(nullptr, vec2[0]);

        vec2.swap(vec3);
        EXPECT_EQ(3, vec2.size());
        EXPECT_EQ(reinterpret_cast<void*>((intptr_t)0xcdcdcdcd), vec2.back());
        EXPECT_EQ(42, vec3.size());
        EXPECT_EQ(nullptr, vec3.back());

        vec3.swap(vec1);
    }

    TEST_F(Arrays, Array)
    {
        // ArrayContainerTest-Begin
        array<int, 10> myArr = {
            {1, 2, 3, 4}
        };
        AZ_TEST_ASSERT(myArr.empty() == false);
        AZ_TEST_ASSERT(myArr.data() != 0);
        AZ_TEST_ASSERT(myArr.size() == 10);
        AZ_TEST_ASSERT(myArr.front() == 1);
        AZ_TEST_ASSERT(myArr.back() == 0);
        AZ_TEST_ASSERT(myArr[1] == 2);
        AZ_TEST_ASSERT(myArr.at(2) == 3);

        using iteratorType = int;
        auto testValue = myArr;
        reverse_iterator<iteratorType*> rend = testValue.rend();
        reverse_iterator<const iteratorType*> crend1 = testValue.rend();
        reverse_iterator<const iteratorType*> crend2 = testValue.crend();

        reverse_iterator<iteratorType*> rbegin = testValue.rbegin();
        reverse_iterator<const iteratorType*> crbegin1 = testValue.rbegin();
        reverse_iterator<const iteratorType*> crbegin2 = testValue.crbegin();

        AZ_TEST_ASSERT(rend == crend1);
        AZ_TEST_ASSERT(crend1 == crend2);

        AZ_TEST_ASSERT(rbegin == crbegin1);
        AZ_TEST_ASSERT(crbegin1 == crbegin2);

        AZ_TEST_ASSERT(rbegin != rend);

        array<int, 10> myArr1 = {
            {10, 11, 12, 13}
        };
        AZ_TEST_ASSERT(myArr != myArr1);
        myArr = myArr1;
        AZ_TEST_ASSERT(myArr == myArr1);

        AZ_TEST_ASSERT(myArr.front() == 10);
        AZ_TEST_ASSERT(myArr.back() == 0);

        myArr1.fill(33);
        AZ_TEST_ASSERT(myArr1.front() == 33);
        AZ_TEST_ASSERT(myArr1.back() == 33);

        myArr.swap(myArr1);
        AZ_TEST_ASSERT(myArr.front() == 33);
        AZ_TEST_ASSERT(myArr.back() == 33);
        AZ_TEST_ASSERT(myArr1.front() == 10);
        AZ_TEST_ASSERT(myArr1.back() == 0);
        // ArrayContainerTest-End
    }

    TEST_F(Arrays, ZeroLengthArray)
    {
        // ArrayContainerTest-Begin
        array<int, 0> myArr;
        EXPECT_TRUE(myArr.empty());
        EXPECT_EQ(0, myArr.size());
        EXPECT_EQ(0, myArr.max_size());
        AZ_TEST_START_TRACE_SUPPRESSION;
        myArr.front();
        myArr.back();
        myArr.at(0);
        myArr[0];
        AZ_TEST_STOP_TRACE_SUPPRESSION(4);

        array<int, 0> myArr2;
        EXPECT_EQ(myArr, myArr2);

        myArr.data();
        myArr.fill(33);
        myArr.swap(myArr2);
        EXPECT_EQ(myArr.begin(), myArr.end());
        EXPECT_EQ(myArr.cbegin(), myArr.cend());
        EXPECT_EQ(myArr.rbegin(), myArr.rend());
        EXPECT_EQ(myArr.crbegin(), myArr.crend());

        // ArrayContainerTest-End
    }

    TEST_F(Arrays, VectorDeepCopy)
    {
        struct MyDeepClass
        {
            MyDeepClass(int value = 10)
                : m_moved(false)
                , m_data(value)
                , m_intVector(10, value + 1)
            {}
            MyDeepClass(const MyDeepClass& rhs)
                : m_moved(rhs.m_moved)
                , m_data(rhs.m_data)
                , m_intVector(rhs.m_intVector)
            {}
            MyDeepClass(MyDeepClass&& rhs)
            {
                m_moved = true;
                m_data = rhs.m_data;
                m_intVector = AZStd::move(rhs.m_intVector);
            }

            MyDeepClass& operator=(const MyDeepClass& rhs)
            {
                m_moved = rhs.m_moved;
                m_data = rhs.m_data;
                m_intVector = rhs.m_intVector;
                return *this;
            }

            bool m_moved;
            int m_data;
            vector<int>     m_intVector;
        };

        typedef vector<MyDeepClass>  deep_vector_type;
        deep_vector_type deep_vec_1;
        AZ_TEST_VALIDATE_EMPTY_VECTOR(deep_vec_1);

        deep_vector_type deep_vec_2(10);
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 10);
        for (size_t i = 0; i < deep_vec_2.size(); ++i)
        {
            AZ_TEST_ASSERT(deep_vec_2[i].m_moved == false);
        }

        // reserve some space
        deep_vec_2.set_capacity(15);

        for (size_t i = 0; i < deep_vec_2.size(); ++i)
        {
            AZ_TEST_ASSERT(deep_vec_2[i].m_moved == true);
        }

        // insert at the end
        deep_vec_2.insert(deep_vec_2.end(), MyDeepClass(100));
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 11);
        AZ_TEST_ASSERT(deep_vec_2.back().m_data == 100);
        AZ_TEST_ASSERT(deep_vec_2.back().m_intVector.size() == 10);

        // insert with unitialized_copy
        deep_vec_2.insert(prior(deep_vec_2.end()), MyDeepClass(200));
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 12);
        AZ_TEST_ASSERT(deep_vec_2.back().m_data == 100);
        AZ_TEST_ASSERT(deep_vec_2.back().m_intVector.size() == 10);

        // insert with uninitilized_copy and move
        deep_vec_2.insert(prior(deep_vec_2.end(), 2), MyDeepClass(300));
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 13);
        AZ_TEST_ASSERT(deep_vec_2.back().m_data == 100);
        AZ_TEST_ASSERT(deep_vec_2.back().m_intVector.size() == 10);

        deep_vec_2.erase(deep_vec_2.begin());
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 12);

        deep_vec_2.clear();
        AZ_TEST_VALIDATE_VECTOR(deep_vec_2, 0);
    }

    TEST_F(Arrays, BitSet)
    {
        // BitsetTest-Begin
        typedef bitset<25> bitset25_type;

        bitset25_type bs;
        AZ_TEST_ASSERT(bs.count() == 0);

        bitset25_type bs1((unsigned long)5);
        AZ_TEST_ASSERT(bs1.count() == 2);
        AZ_TEST_ASSERT(bs1[0] && bs1[2]);

        string str("10110");
        bitset25_type bs2(str, 0, str.length());
        AZ_TEST_ASSERT(bs2.count() == 3);
        AZ_TEST_ASSERT(bs2[1] && bs2[2] && bs2[4]);

        bitset25_type::reference bit0 = bs2[0], bit1 = bs2[1];
        AZ_TEST_ASSERT(bit0 == false);
        AZ_TEST_ASSERT(bit1 == true);

        bs &= bs1;
        AZ_TEST_ASSERT(bs.count() == 0);

        bs |= bs1;
        AZ_TEST_ASSERT(bs.count() == 2);
        AZ_TEST_ASSERT(bs[0] && bs[2]);

        bs ^= bs2;
        AZ_TEST_ASSERT(bs.count() == 3);
        AZ_TEST_ASSERT(bs[0] && bs[1] && bs[4]);

        bs <<= 4;
        AZ_TEST_ASSERT(bs.count() == 3);
        AZ_TEST_ASSERT(bs[4] && bs[5] && bs[8]);

        bs >>= 3;
        AZ_TEST_ASSERT(bs.count() == 3);
        AZ_TEST_ASSERT(bs[1] && bs[2] && bs[5]);

        bs.set(3);
        AZ_TEST_ASSERT(bs.count() == 4);
        AZ_TEST_ASSERT(bs[1] && bs[2] && bs[3] && bs[5]);

        bs.set(1, false);
        AZ_TEST_ASSERT(bs.count() == 3);
        AZ_TEST_ASSERT(!bs[1] && bs[2] && bs[3] && bs[5]);

        bs.set();
        AZ_TEST_ASSERT(bs.count() == 25);

        bs.reset();
        AZ_TEST_ASSERT(bs.count() == 0);

        bs.set(0);
        bs.set(1);
        AZ_TEST_ASSERT(bs.count() == 2);

        bs.flip();
        AZ_TEST_ASSERT(bs.count() == 23);

        bs.flip(0);
        AZ_TEST_ASSERT(bs.count() == 24);

        str = bs.to_string<char>();
        AZ_TEST_ASSERT(str.length() == 25);

        AZ_TEST_ASSERT(bs != bs1);
        bs2 = bs;
        AZ_TEST_ASSERT(bs == bs2);

        bs1.reset();
        AZ_TEST_ASSERT(bs.any());
        AZ_TEST_ASSERT(!bs1.any());
        AZ_TEST_ASSERT(!bs.none());
        AZ_TEST_ASSERT(bs1.none());

        bs1 = bs >> 1;
        AZ_TEST_ASSERT(bs1.count() == 23);

        bs1 = bs << 2;
        AZ_TEST_ASSERT(bs1.count() == 22);

        // extensions
        bitset25_type bs3(string("10110"));
        AZ_TEST_ASSERT(bs3.num_words() == 1); // check number of words
        bitset25_type::word_t tempWord = *bs3.data(); // access the bits data
        AZ_TEST_ASSERT((tempWord & 0x16) == 0x16); // check values
        bitset25_type bs4;
        *bs4.data() = tempWord; // modify the data directly
        AZ_TEST_ASSERT(bs3 == bs4);

        // BitsetTest-End
    }
}
