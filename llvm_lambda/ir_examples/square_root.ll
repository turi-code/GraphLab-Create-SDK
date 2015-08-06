; ModuleID = '<string>'
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin14.3.0"

@PyExc_RuntimeError = external global i8
@".const.<lambda>" = internal constant [9 x i8] c"<lambda>\00"
@".const.Fatal_error:_missing__dynfunc.Closure" = internal constant [38 x i8] c"Fatal error: missing _dynfunc.Closure\00"
@.const.missing_Environment = internal constant [20 x i8] c"missing Environment\00"

; Function Attrs: nounwind
define i32 @"__main__.<lambda>$4.float64"(double* nocapture %retptr, { i8*, i32 }** nocapture readnone %excinfo, i8* nocapture readnone %env, double %arg.x) #0 {
entry:
  %.16 = tail call double @sqrt(double %arg.x)
  store double %.16, double* %retptr, align 8
  ret i32 0
}

; Function Attrs: nounwind readonly
declare double @sqrt(double) #1

define i8* @"wrapper.__main__.<lambda>$4.float64"(i8* %py_closure, i8* %py_args, i8* nocapture readnone %py_kws) {
entry:
  %.5 = alloca i8*, align 8
  %.6 = call i32 (i8*, i8*, i64, i64, ...)* @PyArg_UnpackTuple(i8* %py_args, i8* getelementptr inbounds ([9 x i8]* @".const.<lambda>", i64 0, i64 0), i64 1, i64 1, i8** %.5)
  %.7 = icmp eq i32 %.6, 0
  br i1 %.7, label %entry.if, label %entry.endif, !prof !0

entry.if:                                         ; preds = %entry.endif, %entry
  ret i8* null

entry.endif:                                      ; preds = %entry
  %.11 = load i8** %.5, align 8
  %.12 = call i8* @PyNumber_Float(i8* %.11)
  %.13 = call double @PyFloat_AsDouble(i8* %.12)
  call void @Py_DecRef(i8* %.12)
  %.15 = call i8* @PyErr_Occurred()
  %.16 = icmp eq i8* %.15, null
  br i1 %.16, label %entry.endif.endif, label %entry.if, !prof !1

entry.endif.endif:                                ; preds = %entry.endif
  %.20 = call i8* @PyEval_SaveThread()
  %.21 = icmp eq i8* %py_closure, null
  br i1 %.21, label %entry.endif.endif.if, label %entry.endif.endif.endif, !prof !0

entry.endif.endif.if:                             ; preds = %entry.endif.endif
  %.23 = call i32 @puts(i8* getelementptr inbounds ([38 x i8]* @".const.Fatal_error:_missing__dynfunc.Closure", i64 0, i64 0))
  unreachable

entry.endif.endif.endif:                          ; preds = %entry.endif.endif
  %.25 = ptrtoint i8* %py_closure to i64
  %.26 = add i64 %.25, 24
  %.28 = inttoptr i64 %.26 to { i8* }*
  %.291 = bitcast { i8* }* %.28 to i8**
  %.30 = load i8** %.291, align 8
  %.35 = icmp eq i8* %.30, null
  br i1 %.35, label %entry.endif.endif.endif.if, label %entry.endif.endif.endif.endif, !prof !0

entry.endif.endif.endif.if:                       ; preds = %entry.endif.endif.endif
  call void @PyErr_SetString(i8* @PyExc_RuntimeError, i8* getelementptr inbounds ([20 x i8]* @.const.missing_Environment, i64 0, i64 0))
  ret i8* null

entry.endif.endif.endif.endif:                    ; preds = %entry.endif.endif.endif
  %.16.i = call double @sqrt(double %.13) #0
  call void @PyEval_RestoreThread(i8* %.20)
  %.57 = call i8* @PyFloat_FromDouble(double %.16.i)
  ret i8* %.57
}

declare i32 @PyArg_UnpackTuple(i8*, i8*, i64, i64, ...)

declare i8* @PyNumber_Float(i8*)

declare double @PyFloat_AsDouble(i8*)

declare void @Py_DecRef(i8*)

declare i8* @PyErr_Occurred()

declare i8* @PyEval_SaveThread()

; Function Attrs: nounwind
declare i32 @puts(i8* nocapture readonly) #0

declare void @PyErr_SetString(i8*, i8*)

declare void @PyEval_RestoreThread(i8*)

declare i8* @PyFloat_FromDouble(double)

attributes #0 = { nounwind }
attributes #1 = { nounwind readonly }

!0 = !{!"branch_weights", i32 1, i32 99}
!1 = !{!"branch_weights", i32 99, i32 1}
