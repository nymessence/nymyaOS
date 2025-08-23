/* Kernel module implementation */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init nymya_init(void) {
    pr_info("Nymya Core: Module loaded\n");
    return 0;
}

static void __exit nymya_exit(void) {
    pr_info("Nymya Core: Module unloaded\n");
}

module_init(nymya_init);
module_exit(nymya_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NymyaOS Team");
MODULE_DESCRIPTION("NymyaOS Core Kernel Module");
