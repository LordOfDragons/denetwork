/**
 * MIT License
 *
 * Copyright (c) 2022 DragonDreams (info@dragondreams.ch)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package ch.dragondreams.denetwork.protocol;

/**
 * State value types.
 */
public enum ValueTypes {
	/**
	 * integer: signed char (8-bit).
	 */
	SINT8(0),

	/**
	 * integer: unsigned char (8-bit).
	 */
	UINT8(1),

	/**
	 * integer: signed short (16-bit).
	 */
	SINT16(2),

	/**
	 * integer: unsigned short (16-bit).
	 */
	UINT16(3),

	/**
	 * integer: signed long (32-bit).
	 */
	SINT32(4),

	/**
	 * integer: unsigned long (32-bit).
	 */
	UINT32(5),

	/**
	 * integer: signed long (64-bit).
	 */
	SINT64(6),

	/**
	 * integer: unsigned long (64-bit).
	 */
	UINT64(7),

	/**
	 * float: half float (16-bit).
	 */
	FLOAT16(8),

	/**
	 * float: float (32-bit).
	 */
	FLOAT32(9),

	/**
	 * float: float (32-bit).
	 */
	FLOAT64(10),

	/**
	 * string.
	 */
	STRING(11),

	/**
	 * data: length unsigned 8-bit
	 */
	DATA(12),

	/**
	 * point2: signed 8-bit per component.
	 */
	POINT2S8(13),

	/**
	 * point2: unsigned 8-bit per component.
	 */
	POINT2U8(14),

	/**
	 * point2: signed 16-bit per component.
	 */
	POINT2S16(15),

	/**
	 * point2: unsigned 16-bit per component.
	 */
	POINT2U16(16),

	/**
	 * point2: signed 32-bit per component.
	 */
	POINT2S32(17),

	/**
	 * point2: unsigned 32-bit per component.
	 */
	POINT2U32(18),

	/**
	 * point2: signed 64-bit per component.
	 */
	POINT2S64(19),

	/**
	 * point2: unsigned 64-bit per component.
	 */
	POINT2U64(20),

	/**
	 * point3: signed 8-bit per component.
	 */
	POINT3S8(21),

	/**
	 * point3: unsigned 8-bit per component.
	 */
	POINT3U8(22),

	/**
	 * point3: signed 16-bit per component.
	 */
	POINT3S16(23),

	/**
	 * point3: unsigned 16-bit per component.
	 */
	POINT3U16(24),

	/**
	 * point3: signed 32-bit per component.
	 */
	POINT3S32(25),

	/**
	 * point3: unsigned 32-bit per component.
	 */
	POINT3U32(26),

	/**
	 * point3: signed 64-bit per component.
	 */
	POINT3S64(27),

	/**
	 * point3: unsigned 64-bit per component.
	 */
	POINT3U64(28),

	/**
	 * vector2: 16-bit per component.
	 */
	VECTOR2F16(29),

	/**
	 * vector2: 32-bit per component.
	 */
	VECTOR2F32(30),

	/**
	 * vector2: 64-bit per component.
	 */
	VECTOR2F64(31),

	/**
	 * vector3: 16-bit per component.
	 */
	VECTOR3F16(32),

	/**
	 * vector3: 32-bit per component.
	 */
	VECTOR3F32(33),

	/**
	 * vector3: 64-bit per component.
	 */
	VECTOR3F64(34),

	/**
	 * quaternion: 16-bit per component.
	 */
	QUATERNIONF16(35),

	/**
	 * quaternion: 32-bit per component.
	 */
	QUATERNIONF32(36),

	/**
	 * quaternion: 64-bit per component;.
	 */
	QUATERNIONF64(37);

	public final int value;

	private ValueTypes(int value) {
		this.value = value;
	}

	public static ValueTypes withValue(int value) {
		for (ValueTypes each : values()) {
			if (each.value == value) {
				return each;
			}
		}
		return null;
	}
}
