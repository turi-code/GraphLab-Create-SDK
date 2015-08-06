; ModuleID = '<string>'
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin14.3.0"

@PyExc_RuntimeError = external global i8
@.const.square = internal constant [7 x i8] c"square\00"
@".const.Fatal_error:_missing__dynfunc.Closure" = internal constant [38 x i8] c"Fatal error: missing _dynfunc.Closure\00"
@.const.missing_Environment = internal constant [20 x i8] c"missing Environment\00"

; Function Attrs: nounwind
define i32 @"__main__.square$4.int64"(i64* nocapture %retptr, { i8*, i32 }** nocapture readnone %excinfo, i8* nocapture readnone %env, i64 %arg.x) #0 {
entry:
  %.13 = sitofp i64 %arg.x to double
  %.15 = tail call double @llvm.powi.f64(double %.13, i32 2)
  %.16 = fptosi double %.15 to i64
  store i64 %.16, i64* %retptr, align 8
  ret i32 0
}

; Function Attrs: nounwind readnone
declare double @llvm.powi.f64(double, i32) #1

define i8* @"wrapper.__main__.square$4.int64"(i8* %py_closure, i8* %py_args, i8* nocapture readnone %py_kws) {
entry:
  %.5 = alloca i8*, align 8
  %.6 = call i32 (i8*, i8*, i64, i64, ...)* @PyArg_UnpackTuple(i8* %py_args, i8* getelementptr inbounds ([7 x i8]* @.const.square, i64 0, i64 0), i64 1, i64 1, i8** %.5)
  %.7 = icmp eq i32 %.6, 0
  br i1 %.7, label %entry.if, label %entry.endif, !prof !0

entry.if:                                         ; preds = %entry.endif.endif.endif.endif.endif, %entry.endif.endif, %entry
  %merge = phi i8* [ null, %entry.endif.endif ], [ null, %entry ], [ %.68, %entry.endif.endif.endif.endif.endif ]
  ret i8* %merge

entry.endif:                                      ; preds = %entry
  %.11 = load i8** %.5, align 8
  %.13 = call i8* @PyNumber_Long(i8* %.11)
  %.14 = icmp eq i8* %.13, null
  br i1 %.14, label %entry.endif.endif, label %entry.endif.if, !prof !0

entry.endif.if:                                   ; preds = %entry.endif
  %.16 = call i64 @PyLong_AsLongLong(i8* %.13)
  call void @Py_DecRef(i8* %.13)
  %phitmp = sitofp i64 %.16 to double
  br label %entry.endif.endif

entry.endif.endif:                                ; preds = %entry.endif, %entry.endif.if
  %.12.0 = phi double [ %phitmp, %entry.endif.if ], [ 0.000000e+00, %entry.endif ]
  %.21 = call i8* @PyErr_Occurred()
  %.22 = icmp eq i8* %.21, null
  br i1 %.22, label %entry.endif.endif.endif, label %entry.if, !prof !1

entry.endif.endif.endif:                          ; preds = %entry.endif.endif
  %.26 = call i8* @PyEval_SaveThread()
  %.27 = icmp eq i8* %py_closure, null
  br i1 %.27, label %entry.endif.endif.endif.if, label %entry.endif.endif.endif.endif, !prof !0

entry.endif.endif.endif.if:                       ; preds = %entry.endif.endif.endif
  %.29 = call i32 @puts(i8* getelementptr inbounds ([38 x i8]* @".const.Fatal_error:_missing__dynfunc.Closure", i64 0, i64 0))
  unreachable

entry.endif.endif.endif.endif:                    ; preds = %entry.endif.endif.endif
  %.31 = ptrtoint i8* %py_closure to i64
  %.32 = add i64 %.31, 24
  %.34 = inttoptr i64 %.32 to { i8* }*
  %.351 = bitcast { i8* }* %.34 to i8**
  %.36 = load i8** %.351, align 8
  %.41 = icmp eq i8* %.36, null
  br i1 %.41, label %entry.endif.endif.endif.endif.if, label %entry.endif.endif.endif.endif.endif, !prof !0

entry.endif.endif.endif.endif.if:                 ; preds = %entry.endif.endif.endif.endif
  call void @PyErr_SetString(i8* @PyExc_RuntimeError, i8* getelementptr inbounds ([20 x i8]* @.const.missing_Environment, i64 0, i64 0))
  ret i8* null

entry.endif.endif.endif.endif.endif:              ; preds = %entry.endif.endif.endif.endif
  %.15.i = call double @llvm.powi.f64(double %.12.0, i32 2) #0
  %.16.i = fptosi double %.15.i to i64
  call void @PyEval_RestoreThread(i8* %.26)
  %.68 = call i8* @PyInt_FromLong(i64 %.16.i)
  br label %entry.if
}

declare i32 @PyArg_UnpackTuple(i8*, i8*, i64, i64, ...)

declare i8* @PyNumber_Long(i8*)

declare i64 @PyLong_AsLongLong(i8*)

declare void @Py_DecRef(i8*)

declare i8* @PyErr_Occurred()

declare i8* @PyEval_SaveThread()

; Function Attrs: nounwind
declare i32 @puts(i8* nocapture readonly) #0

declare void @PyErr_SetString(i8*, i8*)

declare void @PyEval_RestoreThread(i8*)

declare i8* @PyInt_FromLong(i64)

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }

!0 = !{!"branch_weights", i32 1, i32 99}
!1 = !{!"branch_weights", i32 99, i32 1}
