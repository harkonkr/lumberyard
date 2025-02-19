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

#pragma once

// include the required headers
#include <AzCore/RTTI/TypeInfo.h>
#include "StandardHeaders.h"
#include "Quaternion.h"
#include "Algorithms.h"


namespace MCore
{
    /**
     * The compressed / packed quaternion class.
     * This represents a unit (normalized) quaternion on a packed way, which is 8 bytes per quaternion instead of 16 bytes
     * when using floating point quaternions. Of course we loose a bit of precision, but it isn't too bad for most things.
     * The class provides methods to convert from and to uncompressed quaternions.
     */
    template <class StorageType>
    class TCompressedQuaternion
    {
    public:
        AZ_TYPE_INFO(MCore::TCompressedQuaternion, "{31AD5C7F-A999-40C5-AC3A-E13D150036E3}", StorageType)

        /**
         * Default constructor.
         * This sets the quaternion to identity.
         */
        MCORE_INLINE TCompressedQuaternion();

        /**
         * Create a compressed quaternion from compressed x, y, z, w values.
         * @param xVal The value of x.
         * @param yVal The value of y.
         * @param zVal The value of z.
         * @param wVal The value of w.
         */
        MCORE_INLINE TCompressedQuaternion(float xVal, float yVal, float zVal, float wVal);

        /**
         * Create a compressed quaternion from an uncompressed one.
         * Please note that the uncompressed quaternion has to be normalized or a unit quaternion!
         * @param quat The normalized uncompressed quaternion.
         */
        MCORE_INLINE TCompressedQuaternion(const Quaternion& quat);

        /**
         * Update the compressed quaternion from an uncompressed one.
         * Please note that the uncompressed quaternion has to be normalized or a unit quaternion!
         * @param quat The normalized uncompressed quaternion.
         */
        MCORE_INLINE void FromQuaternion(const Quaternion& quat);

        /**
         * Uncompress the compressed quaternion into an uncompressed one.
         * @param output The output uncompressed quaternion to write the result in.
         */
        MCORE_INLINE void UnCompress(Quaternion* output) const;

        /**
         * Convert the compressed quaternion into an uncompressed one.
         * This method works the same as the UnCompress method, but it returns the result instead of specifying
         * the output quaternion.
         * @result The uncompressed version of this compressed quaternion.
         */
        MCORE_INLINE Quaternion ToQuaternion() const;

    public:
        StorageType mX, mY, mZ, mW; /**< The compressed/packed quaternion components values. */

        // the number of steps within the specified range
        enum
        {
            CONVERT_VALUE = ((1 << (sizeof(StorageType) << 3)) >> 1) - 1
        };
    };

    // include the inline code
#include "CompressedQuaternion.inl"

    // declare standard types
    typedef TCompressedQuaternion<int16>    Compressed16BitQuaternion;
    typedef TCompressedQuaternion<int8>     Compressed8BitQuaternion;
} // namespace MCore
