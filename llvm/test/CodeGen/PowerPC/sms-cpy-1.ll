; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=powerpc64le-unknown-linux-gnu -verify-machineinstrs\
; RUN:       -mcpu=pwr9 --ppc-enable-pipeliner 2>&1 | FileCheck %s

@.str.28 = external unnamed_addr constant [69 x i8], align 1

define void @print_res(ptr %p) nounwind {
; CHECK-LABEL: print_res:
; CHECK:       # %bb.0:
; CHECK-NEXT:    lwz 4, 0(3)
; CHECK-NEXT:    addi 4, 4, -1
; CHECK-NEXT:    clrldi 5, 4, 32
; CHECK-NEXT:    cmplwi 4, 3
; CHECK-NEXT:    li 4, 3
; CHECK-NEXT:    isellt 4, 5, 4
; CHECK-NEXT:    li 5, 1
; CHECK-NEXT:    cmpldi 4, 1
; CHECK-NEXT:    iselgt 4, 4, 5
; CHECK-NEXT:    li 5, 0
; CHECK-NEXT:    mtctr 4
; CHECK-NEXT:    li 8, -1
; CHECK-NEXT:    lbz 6, 0(3)
; CHECK-NEXT:    li 4, 1
; CHECK-NEXT:    bdz .LBB0_6
; CHECK-NEXT:  # %bb.1:
; CHECK-NEXT:    xori 7, 6, 84
; CHECK-NEXT:    clrldi 6, 8, 32
; CHECK-NEXT:    addi 4, 4, 1
; CHECK-NEXT:    addi 9, 8, -1
; CHECK-NEXT:    lbzx 6, 3, 6
; CHECK-NEXT:    bdz .LBB0_5
; CHECK-NEXT:  # %bb.2:
; CHECK-NEXT:    cntlzw 7, 7
; CHECK-NEXT:    addi 4, 4, 1
; CHECK-NEXT:    srwi 8, 7, 5
; CHECK-NEXT:    xori 7, 6, 84
; CHECK-NEXT:    clrldi 6, 9, 32
; CHECK-NEXT:    addi 9, 9, -1
; CHECK-NEXT:    lbzx 6, 3, 6
; CHECK-NEXT:    bdz .LBB0_4
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  .LBB0_3:
; CHECK-NEXT:    clrldi 11, 9, 32
; CHECK-NEXT:    cntlzw 10, 7
; CHECK-NEXT:    xori 7, 6, 84
; CHECK-NEXT:    addi 9, 9, -1
; CHECK-NEXT:    lbzx 6, 3, 11
; CHECK-NEXT:    addi 4, 4, 1
; CHECK-NEXT:    add 5, 5, 8
; CHECK-NEXT:    srwi 8, 10, 5
; CHECK-NEXT:    bdnz .LBB0_3
; CHECK-NEXT:  .LBB0_4:
; CHECK-NEXT:    add 5, 5, 8
; CHECK-NEXT:  .LBB0_5:
; CHECK-NEXT:    cntlzw 3, 7
; CHECK-NEXT:    srwi 3, 3, 5
; CHECK-NEXT:    add 5, 5, 3
; CHECK-NEXT:  .LBB0_6:
; CHECK-NEXT:    xori 3, 6, 84
; CHECK-NEXT:    mflr 0
; CHECK-NEXT:    cntlzw 3, 3
; CHECK-NEXT:    srwi 3, 3, 5
; CHECK-NEXT:    add 3, 5, 3
; CHECK-NEXT:    stdu 1, -128(1)
; CHECK-NEXT:    clrldi 6, 3, 32
; CHECK-NEXT:    addis 3, 2, .LC0@toc@ha
; CHECK-NEXT:    li 5, 0
; CHECK-NEXT:    std 0, 144(1)
; CHECK-NEXT:    ld 3, .LC0@toc@l(3)
; CHECK-NEXT:    std 5, 120(1)
; CHECK-NEXT:    li 5, 3
; CHECK-NEXT:    clrldi 4, 4, 32
; CHECK-NEXT:    std 4, 104(1)
; CHECK-NEXT:    li 4, 3
; CHECK-NEXT:    li 7, 0
; CHECK-NEXT:    li 8, 3
; CHECK-NEXT:    li 10, 0
; CHECK-NEXT:    std 5, 96(1)
; CHECK-NEXT:    li 5, 0
; CHECK-NEXT:    bl printf
; CHECK-NEXT:    nop
  %1 = load i32, ptr undef, align 4
  %2 = add i32 %1, -1
  %3 = zext i32 %2 to i64
  %4 = zext i32 3 to i64
  br label %5

5:                                                ; preds = %5, %0
  %6 = phi i64 [ %16, %5 ], [ 0, %0 ]
  %7 = phi i32 [ %15, %5 ], [ 0, %0 ]
  %8 = trunc i64 %6 to i32
  %9 = sub i32 0, %8
  %10 = zext i32 %9 to i64
  %11 = getelementptr inbounds i8, ptr %p, i64 %10
  %12 = load i8, ptr %11, align 1
  %13 = icmp eq i8 %12, 84
  %14 = zext i1 %13 to i32
  %15 = add i32 %7, %14
  %16 = add nuw nsw i64 %6, 1
  %17 = icmp ult i64 %16, %3
  %18 = icmp ult i64 %16, %4
  %19 = and i1 %18, %17
  br i1 %19, label %5, label %20

20:                                               ; preds = %5
  %21 = trunc i64 %16 to i32
  call void (ptr, ...) @printf(ptr @.str.28, i32 zeroext 3, i32 zeroext undef, i32 zeroext %15, i32 zeroext undef, i32 zeroext 3, ptr undef, i32 zeroext undef, i32 zeroext 3, i32 zeroext %21, ptr undef, i32 zeroext undef) #1
  unreachable
}

declare void @printf(ptr, ...) local_unnamed_addr #0
