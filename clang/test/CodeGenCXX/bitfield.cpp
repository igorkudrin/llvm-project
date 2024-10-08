// RUN: %clang_cc1 -triple x86_64-unknown-unknown -fdump-record-layouts-simple \
// RUN:   -emit-llvm -o %t %s | FileCheck -check-prefixes=LAYOUT,LAYOUT-X86-64 %s
// RUN: FileCheck -check-prefix=CHECK-X86-64 %s <%t
// RUN: %clang_cc1 -triple powerpc64-unknown-unknown -fdump-record-layouts-simple\
// RUN:  -emit-llvm -o %t %s | FileCheck -check-prefixes=LAYOUT,LAYOUT-PPC64 %s
// RUN: FileCheck -check-prefix=CHECK-PPC64 %s <%t
//
// Tests for bitfield access patterns in C++ with special attention to
// conformance to C++11 memory model requirements.

namespace N0 {
  // Test basic bitfield layout access across interesting byte and word
  // boundaries on both little endian and big endian platforms.
  struct __attribute__((packed)) S {
    unsigned b00 : 14;
    unsigned b01 : 2;
    unsigned b20 : 6;
    unsigned b21 : 2;
    unsigned b30 : 30;
    unsigned b31 : 2;
    unsigned b70 : 6;
    unsigned b71 : 2;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N0::S" =
// LAYOUT-SAME: type { i64 }
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:14 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:14 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:16 Size:6 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:22 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:24 Size:30 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:54 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:56 Size:6 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:62 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:50 Size:14 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:48 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:42 Size:6 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:40 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:10 Size:30 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:8 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:2 Size:6 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:2 IsSigned:0 StorageSize:64 StorageOffset:0
// LAYOUT-NEXT: ]>

  unsigned read00(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read00
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[val]], 16383
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read00
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 50
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[shr]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b00;
  }
  unsigned read01(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read01
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 14
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read01
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 48
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b01;
  }
  unsigned read20(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read20
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 16
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 63
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read20
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 42
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 63
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b20;
  }
  unsigned read21(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read21
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 22
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read21
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 40
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b21;
  }
  unsigned read30(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read30
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 24
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 1073741823
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read30
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 10
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 1073741823
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b30;
  }
  unsigned read31(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read31
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 54
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read31
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 8
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 3
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b31;
  }
  unsigned read70(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read70
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 56
    // CHECK-X86-64:   %[[and:.*]]   = and i64 %[[shr]], 63
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read70
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]]   = lshr i64 %[[val]], 2
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[shr]], 63
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b70;
  }
  unsigned read71(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N06read71
    // CHECK-X86-64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-X86-64:   %[[shr:.*]]   = lshr i64 %[[val]], 62
    // CHECK-X86-64:   %[[trunc:.*]] = trunc i64 %[[shr]] to i32
    // CHECK-X86-64:                   ret i32 %[[trunc]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N06read71
    // CHECK-PPC64:   %[[val:.*]]   = load i64, ptr %{{.*}}
    // CHECK-PPC64:   %[[and:.*]]   = and i64 %[[val]], 3
    // CHECK-PPC64:   %[[trunc:.*]] = trunc i64 %[[and]] to i32
    // CHECK-PPC64:                   ret i32 %[[trunc]]
    return s->b71;
  }
}

namespace N1 {
  // Ensure that neither loads nor stores to bitfields are not widened into
  // other memory locations. (PR13691)
  //
  // NOTE: We could potentially widen loads based on their alignment if we are
  // comfortable requiring that subsequent memory locations within the
  // alignment-widened load are not volatile.
  struct S {
    char a;
    unsigned b : 1;
    char c;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N1::S" =
// LAYOUT-SAME: type { i8, i8, i8, i8 }
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:1 IsSigned:0 StorageSize:8 StorageOffset:1
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:7 Size:1 IsSigned:0 StorageSize:8 StorageOffset:1
// LAYOUT-NEXT: ]>

  unsigned read(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N14read
    // CHECK-X86-64:   %[[ptr:.*]] = getelementptr inbounds nuw %{{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[val:.*]] = load i8, ptr %[[ptr]]
    // CHECK-X86-64:   %[[and:.*]] = and i8 %[[val]], 1
    // CHECK-X86-64:   %[[ext:.*]] = zext i8 %[[and]] to i32
    // CHECK-X86-64:                 ret i32 %[[ext]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N14read
    // CHECK-PPC64:   %[[ptr:.*]] = getelementptr inbounds nuw %{{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[val:.*]] = load i8, ptr %[[ptr]]
    // CHECK-PPC64:   %[[shr:.*]] = lshr i8 %[[val]], 7
    // CHECK-PPC64:   %[[ext:.*]] = zext i8 %[[shr]] to i32
    // CHECK-PPC64:                 ret i32 %[[ext]]
    return s->b;
  }
  void write(S* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N15write
    // CHECK-X86-64:   %[[ptr:.*]]     = getelementptr inbounds nuw %{{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[x_trunc:.*]] = trunc i32 %{{.*}} to i8
    // CHECK-X86-64:   %[[old:.*]]     = load i8, ptr %[[ptr]]
    // CHECK-X86-64:   %[[x_and:.*]]   = and i8 %[[x_trunc]], 1
    // CHECK-X86-64:   %[[old_and:.*]] = and i8 %[[old]], -2
    // CHECK-X86-64:   %[[new:.*]]     = or i8 %[[old_and]], %[[x_and]]
    // CHECK-X86-64:                     store i8 %[[new]], ptr %[[ptr]]
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N15write
    // CHECK-PPC64:   %[[ptr:.*]]     = getelementptr inbounds nuw %{{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[x_trunc:.*]] = trunc i32 %{{.*}} to i8
    // CHECK-PPC64:   %[[old:.*]]     = load i8, ptr %[[ptr]]
    // CHECK-PPC64:   %[[x_and:.*]]   = and i8 %[[x_trunc]], 1
    // CHECK-PPC64:   %[[x_shl:.*]]   = shl i8 %[[x_and]], 7
    // CHECK-PPC64:   %[[old_and:.*]] = and i8 %[[old]], 127
    // CHECK-PPC64:   %[[new:.*]]     = or i8 %[[old_and]], %[[x_shl]]
    // CHECK-PPC64:                     store i8 %[[new]], ptr %[[ptr]]
    s->b = x;
  }
}

namespace N2 {
  // Do widen loads and stores to bitfields when those bitfields have padding
  // within the struct following them.
  struct S {
    unsigned b : 24;
    void *p;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N2::S" =
// LAYOUT-SAME: type { i32, ptr }
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:8 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-NEXT: ]>

  unsigned read(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N24read
    // CHECK-X86-64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-X86-64:   %[[and:.*]] = and i32 %[[val]], 16777215
    // CHECK-X86-64:                 ret i32 %[[and]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N24read
    // CHECK-PPC64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]] = lshr i32 %[[val]], 8
    // CHECK-PPC64:                 ret i32 %[[shr]]
    return s->b;
  }
  void write(S* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N25write
    // CHECK-X86-64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-X86-64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-X86-64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-X86-64:   %[[old_and:.*]] = and i32 %[[old]], -16777216
    // CHECK-X86-64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_and]]
    // CHECK-X86-64:                     store i32 %[[new]], ptr %{{.*}}
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N25write
    // CHECK-PPC64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-PPC64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-PPC64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-PPC64:   %[[x_shl:.*]]   = shl i32 %[[x_and]], 8
    // CHECK-PPC64:   %[[old_and:.*]] = and i32 %[[old]], 255
    // CHECK-PPC64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_shl]]
    // CHECK-PPC64:                     store i32 %[[new]], ptr %{{.*}}
    s->b = x;
  }
}

namespace N3 {
  // Do widen loads and stores to bitfields through the trailing padding at the
  // end of a struct.
  struct S {
    unsigned b : 24;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N3::S" =
// LAYOUT-SAME: type { i32 }
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:8 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-NEXT: ]>

  unsigned read(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N34read
    // CHECK-X86-64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-X86-64:   %[[and:.*]] = and i32 %[[val]], 16777215
    // CHECK-X86-64:                 ret i32 %[[and]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N34read
    // CHECK-PPC64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]] = lshr i32 %[[val]], 8
    // CHECK-PPC64:                 ret i32 %[[shr]]
    return s->b;
  }
  void write(S* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N35write
    // CHECK-X86-64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-X86-64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-X86-64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-X86-64:   %[[old_and:.*]] = and i32 %[[old]], -16777216
    // CHECK-X86-64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_and]]
    // CHECK-X86-64:                     store i32 %[[new]], ptr %{{.*}}
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N35write
    // CHECK-PPC64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-PPC64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-PPC64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-PPC64:   %[[x_shl:.*]]   = shl i32 %[[x_and]], 8
    // CHECK-PPC64:   %[[old_and:.*]] = and i32 %[[old]], 255
    // CHECK-PPC64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_shl]]
    // CHECK-PPC64:                     store i32 %[[new]], ptr %{{.*}}
    s->b = x;
  }
}

namespace N4 {
  // Do NOT widen loads and stores to bitfields into padding at the end of
  // a class which might end up with members inside of it when inside a derived
  // class.
  struct Base {
    virtual ~Base() {}

    unsigned b : 24;
  };
  // Imagine some other translation unit introduces:
#if 0
  struct Derived : public Base {
    char c;
  };
#endif
// LAYOUT-LABEL: LLVMType:%"struct.N4::Base" =
// LAYOUT-SAME: type <{ ptr, [3 x i8], [5 x i8] }>
// LAYOUT-NEXT: NonVirtualBaseLLVMType:%"struct.N4::Base.base" = type <{ ptr, [3 x i8] }>
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-NEXT: ]>

  unsigned read(Base* s) {
    // FIXME: We should widen this load as long as the function isn't being
    // instrumented by ThreadSanitizer.
    //
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N44read
    // CHECK-X86-64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[val:.*]] = load i24, ptr %[[gep]]
    // CHECK-X86-64:   %[[ext:.*]] = zext i24 %[[val]] to i32
    // CHECK-X86-64:                 ret i32 %[[ext]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N44read
    // CHECK-PPC64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[val:.*]] = load i24, ptr %[[gep]]
    // CHECK-PPC64:   %[[ext:.*]] = zext i24 %[[val]] to i32
    // CHECK-PPC64:                 ret i32 %[[ext]]
    return s->b;
  }
  void write(Base* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N45write
    // CHECK-X86-64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[new:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-X86-64:                 store i24 %[[new]], ptr %[[gep]]
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N45write
    // CHECK-PPC64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[new:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-PPC64:                 store i24 %[[new]], ptr %[[gep]]
    s->b = x;
  }
}

namespace N5 {
  // Widen through padding at the end of a struct even if that struct
  // participates in a union with another struct which has a separate field in
  // that location. The reasoning is that if the operation is storing to that
  // member of the union, it must be the active member, and thus we can write
  // through the padding. If it is a load, it might be a load of a common
  // prefix through a non-active member, but in such a case the extra bits
  // loaded are masked off anyways.
  union U {
    struct X { unsigned b : 24; char c; } x;
    struct Y { unsigned b : 24; } y;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N5::U::X" =
// LAYOUT-SAME: type { [3 x i8], i8 }
// LAYOUT-NEXT:  NonVirtualBaseLLVMType:%"struct.N5::U::X" =
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:0
// LAYOUT-NEXT: ]>

// LAYOUT-LABEL: LLVMType:%"struct.N5::U::Y" =
// LAYOUT-SAME: type { i32 }
// LAYOUT-NEXT: NonVirtualBaseLLVMType:%"struct.N5::U::Y" =
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:8 Size:24 IsSigned:0 StorageSize:32 StorageOffset:0
// LAYOUT-NEXT: ]>

  unsigned read(U* u) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N54read
    // CHECK-X86-64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-X86-64:   %[[and:.*]] = and i32 %[[val]], 16777215
    // CHECK-X86-64:                 ret i32 %[[and]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N54read
    // CHECK-PPC64:   %[[val:.*]] = load i32, ptr %{{.*}}
    // CHECK-PPC64:   %[[shr:.*]] = lshr i32 %[[val]], 8
    // CHECK-PPC64:                 ret i32 %[[shr]]
    return u->y.b;
  }
  void write(U* u, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N55write
    // CHECK-X86-64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-X86-64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-X86-64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-X86-64:   %[[old_and:.*]] = and i32 %[[old]], -16777216
    // CHECK-X86-64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_and]]
    // CHECK-X86-64:                     store i32 %[[new]], ptr %{{.*}}
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N55write
    // CHECK-PPC64:   %[[ptr:.*]]     = load ptr, ptr %{{.*}}
    // CHECK-PPC64:   %[[old:.*]]     = load i32, ptr %[[ptr]]
    // CHECK-PPC64:   %[[x_and:.*]]   = and i32 %{{.*}}, 16777215
    // CHECK-PPC64:   %[[x_shl:.*]]   = shl i32 %[[x_and]], 8
    // CHECK-PPC64:   %[[old_and:.*]] = and i32 %[[old]], 255
    // CHECK-PPC64:   %[[new:.*]]     = or i32 %[[old_and]], %[[x_shl]]
    // CHECK-PPC64:                     store i32 %[[new]], ptr %{{.*}}
    u->y.b = x;
  }
}

namespace N6 {
  // Zero-length bitfields partition the memory locations of bitfields for the
  // purposes of the memory model. That means stores must not span zero-length
  // bitfields and loads may only span them when we are not instrumenting with
  // ThreadSanitizer.
  // FIXME: We currently don't widen loads even without ThreadSanitizer, even
  // though we could.
  struct S {
    unsigned b1 : 24;
    unsigned char : 0;
    unsigned char b2 : 8;
  };
// LAYOUT-LABEL: LLVMType:%"struct.N6::S" =
// LAYOUT-SAME: type { [3 x i8], i8 }
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:0
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:8 IsSigned:0 StorageSize:8 StorageOffset:3
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:0
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:8 IsSigned:0 StorageSize:8 StorageOffset:3
// LAYOUT-NEXT: ]>

  unsigned read(S* s) {
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N64read
    // CHECK-X86-64:   %[[val1:.*]] = load i24, ptr %{{.*}}
    // CHECK-X86-64:   %[[ext1:.*]] = zext i24 %[[val1]] to i32
    // CHECK-X86-64:   %[[ptr2:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[val2:.*]] = load i8, ptr %[[ptr2]]
    // CHECK-X86-64:   %[[ext2:.*]] = zext i8 %[[val2]] to i32
    // CHECK-X86-64:   %[[add:.*]]  = add nsw i32 %[[ext1]], %[[ext2]]
    // CHECK-X86-64:                  ret i32 %[[add]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N64read
    // CHECK-PPC64:   %[[val1:.*]] = load i24, ptr %{{.*}}
    // CHECK-PPC64:   %[[ext1:.*]] = zext i24 %[[val1]] to i32
    // CHECK-PPC64:   %[[ptr2:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[val2:.*]] = load i8, ptr %[[ptr2]]
    // CHECK-PPC64:   %[[ext2:.*]] = zext i8 %[[val2]] to i32
    // CHECK-PPC64:   %[[add:.*]]  = add nsw i32 %[[ext1]], %[[ext2]]
    // CHECK-PPC64:                  ret i32 %[[add]]
    return s->b1 + s->b2;
  }
  void write(S* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N65write
    // CHECK-X86-64:   %[[new1:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-X86-64:                  store i24 %[[new1]], ptr %{{.*}}
    // CHECK-X86-64:   %[[new2:.*]] = trunc i32 %{{.*}} to i8
    // CHECK-X86-64:   %[[ptr2:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:                  store i8 %[[new2]], ptr %[[ptr2]]
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N65write
    // CHECK-PPC64:   %[[new1:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-PPC64:                  store i24 %[[new1]], ptr %{{.*}}
    // CHECK-PPC64:   %[[new2:.*]] = trunc i32 %{{.*}} to i8
    // CHECK-PPC64:   %[[ptr2:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:                  store i8 %[[new2]], ptr %[[ptr2]]
    s->b1 = x;
    s->b2 = x;
  }
}

namespace N7 {
  // Similar to N4 except that this adds a virtual base to the picture. (PR18430)
  // Do NOT widen loads and stores to bitfields into padding at the end of
  // a class which might end up with members inside of it when inside a derived
  // class.
  struct B1 {
    virtual void f();
    unsigned b1 : 24;
  };
  struct B2 : virtual B1 {
    virtual ~B2();
    unsigned b : 24;
  };
  // Imagine some other translation unit introduces:
#if 0
  struct Derived : public B2 {
    char c;
  };
#endif
// LAYOUT-LABEL: LLVMType:%"struct.N7::B1" =
// LAYOUT-SAME: type <{ ptr, [3 x i8], [5 x i8] }>
// LAYOUT-NEXT:  NonVirtualBaseLLVMType:%"struct.N7::B1.base" = type <{ ptr, [3 x i8] }>
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-NEXT: ]>

// LAYOUT-LABEL: LLVMType:%"struct.N7::B2" =
// LAYOUT-SAME: type <{ ptr, [3 x i8], [5 x i8], %"struct.N7::B1.base", [5 x i8] }>
// LAYOUT-NEXT: NonVirtualBaseLLVMType:%"struct.N7::B2.base" = type <{ ptr, [3 x i8] }>
// LAYOUT: BitFields:[
// LAYOUT-X86-64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-PPC64-NEXT: <CGBitFieldInfo Offset:0 Size:24 IsSigned:0 StorageSize:24 StorageOffset:8
// LAYOUT-NEXT: ]>

  unsigned read(B2* s) {
    // FIXME: We should widen this load as long as the function isn't being
    // instrumented by ThreadSanitizer.
    //
    // CHECK-X86-64-LABEL: define{{.*}} i32 @_ZN2N74read
    // CHECK-X86-64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[val:.*]] = load i24, ptr %[[gep]]
    // CHECK-X86-64:   %[[ext:.*]] = zext i24 %[[val]] to i32
    // CHECK-X86-64:                 ret i32 %[[ext]]
    // CHECK-PPC64-LABEL: define{{.*}} zeroext i32 @_ZN2N74read
    // CHECK-PPC64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[val:.*]] = load i24, ptr %[[gep]]
    // CHECK-PPC64:   %[[ext:.*]] = zext i24 %[[val]] to i32
    // CHECK-PPC64:                 ret i32 %[[ext]]
    return s->b;
  }
  void write(B2* s, unsigned x) {
    // CHECK-X86-64-LABEL: define{{.*}} void @_ZN2N75write
    // CHECK-X86-64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-X86-64:   %[[new:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-X86-64:                 store i24 %[[new]], ptr %[[gep]]
    // CHECK-PPC64-LABEL: define{{.*}} void @_ZN2N75write
    // CHECK-PPC64:   %[[gep:.*]] = getelementptr inbounds nuw {{.*}}, ptr %{{.*}}, i32 0, i32 1
    // CHECK-PPC64:   %[[new:.*]] = trunc i32 %{{.*}} to i24
    // CHECK-PPC64:                 store i24 %[[new]], ptr %[[gep]]
    s->b = x;
  }
}
