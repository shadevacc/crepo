diff --git a/.gitignore b/.gitignore
index d36b4d9..5c2b09d 100644
--- a/.gitignore
+++ b/.gitignore
@@ -1,2 +1,5 @@
 kcodes/docsldd/_build
 _build
+.DS_Store
+ccodes/.DS_Store
+kcodes/.DS_Store
diff --git a/ccodes/src/callfunc.c b/ccodes/src/callfunc.c
index 2bd9ea0..ae8210f 100755
--- a/ccodes/src/callfunc.c
+++ b/ccodes/src/callfunc.c
@@ -3,11 +3,13 @@
 
 void callfunc()
 {
-    printf("SETUP READY\n");
+    //printf("SETUP READY\n");
 #if 0
     a0_parr();
     ds_stack();
     a2_callgrind();
+    l0_merge(); // Seeing compilation errors and to be fixed
 #endif
-    l0_merge();
+    dsandalgs_call();
+    cpointers_call();
 }
\ No newline at end of file
diff --git a/ccodes/src/leetcode/001_mergeSortedArray.c b/ccodes/src/leetcode/001_mergeSortedArray.c
index 889429b..c6df4e9 100644
--- a/ccodes/src/leetcode/001_mergeSortedArray.c
+++ b/ccodes/src/leetcode/001_mergeSortedArray.c
@@ -17,7 +17,7 @@ void l0_merge(int* nums1, int nums1Size, int m,
         case 'A':
         case 'E':
         case 'I':
-        case 'I':
+        case 'O':
         case 'U':
         {
             printf("character %c is VOWEL\n", ch);
diff --git a/ccodes/src/main.h b/ccodes/src/main.h
index b87f6f1..39273af 100644
--- a/ccodes/src/main.h
+++ b/ccodes/src/main.h
@@ -5,3 +5,5 @@
 void callfunc();
 void a0_parr();
 int ds_stack();
+void cpointers_call();
+void dsandalgs_call();
\ No newline at end of file
diff --git a/kcodes/28_completion/static_completion.c b/kcodes/28_completion/static_completion.c
index 90414f6..ee6d294 100644
--- a/kcodes/28_completion/static_completion.c
+++ b/kcodes/28_completion/static_completion.c
@@ -103,9 +103,14 @@ static int m_thread_fn(void *data) {
     while(1) {
         pr_info("waiting for event completion..\n");
         wait_for_completion(&m_data_read_done);
+	mdelay(2000);
         if (m_completion_var == 2) {
             pr_info("Event came from exit function\n");
-            return EXIT_SUCCESS;
+	    if (kthread_should_stop()) {
+                pr_info("kthread_should_stop is executed\n");
+	        return EXIT_SUCCESS;
+            }
+            //return EXIT_SUCCESS;
         } else if (m_completion_var == 1) {
             pr_info("Event came from read function\n");
         } else {
@@ -289,6 +294,8 @@ static void __exit m_exit(void)
     m_completion_var = 2;
     if (!completion_done(&m_data_read_done))
         complete(&m_data_read_done);
+    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
+    mdelay(1000);
 #if 0
     list_for_each_entry_safe(cursor, temp, &mlist_head, mlist) {
         list_del(&cursor->mlist);
