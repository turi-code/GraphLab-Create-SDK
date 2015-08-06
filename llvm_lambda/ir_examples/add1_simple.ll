; ModuleID = '<string>'
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin14.3.0"

; Function Attrs: nounwind
define i64 @"add1"(i64 %arg.x) #0 {
entry:
  %.13 = add nsw i64 %arg.x, 1
  ret i64 %.13
}
