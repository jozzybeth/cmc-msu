; ModuleID = 'test/demo/demo.py'
source_filename = "test/demo/demo.py"

@"$int.class.prototype" = external externally_initialized constant <{ <{ i32, i32, ptr }>, i32 }>
@"$bool.class.prototype" = external externally_initialized constant <{ <{ i32, i32, ptr }>, i1 }>
@"$str.class.prototype" = external externally_initialized constant <{ <{ <{ i32, i32, ptr }>, i32 }>, [1 x i8] }>
@"$a" = private global i32 0
@"$c" = private global i32 0

declare void @"$abort"(ptr)

declare void @"$print"(ptr)

declare ptr @"$alloc"(ptr)

define void @Main() {
Entry:
  ret void
}
