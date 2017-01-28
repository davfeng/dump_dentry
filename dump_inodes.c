/*
 * dump_inodes - traverse the super block's s_inodes list.
 */

#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/kdev_t.h>

static uint sb_lock = 0;
static uint super_blocks = 0;
static char* fs_name=NULL;
module_param(sb_lock, uint, 0644);
module_param(super_blocks, uint, 0644);
module_param(fs_name, charp, 0644);

static int __init printfs_init(void)
{
	struct super_block *sb;
	struct dentry *pdentry;
	struct inode *pinode;
	
	struct list_head *pos;
	struct list_head *linode;
	struct hlist_node *ldentry;
	
	unsigned long long count = 0;
	if ( super_blocks == 0){
		printk("super_blocks is 0\n");
		return 0;
	} 
	
	if ( sb_lock == 0 ) {
		printk("sb_lock is 0\n");
		return 0;
	}

	printk("print some fields of super blocks, fs_name:%s\n", fs_name);
	spin_lock((spinlock_t *)sb_lock);
	
	printk("super_blocks=0x%x\n",(uint)super_blocks);
	list_for_each(pos, (struct list_head *)super_blocks){
		sb = list_entry(pos, struct super_block, s_list);
		printk("\n");
		printk("sb=0x%p",sb);
		printk(" dev_t:%d,%d ", MAJOR(sb->s_dev), MINOR(sb->s_dev));
		printk(" fs_name:%s",sb->s_type->name);
		{
			char buf[100];
			if(sb->s_bdev)
				printk(" mnt_devname: %s", bdevname(sb->s_bdev, buf));
			printk(" mounted point: d_name len:%d  %s", sb->s_root->d_name.len, sb->s_root->d_name.name);
			printk(" s_id: %s", sb->s_id);
		}
		if((fs_name && strncmp(sb->s_type->name, fs_name, strlen(sb->s_type->name))) || !fs_name)
			continue;
		/*travers the inodes in the super block*/
		list_for_each(linode, &sb->s_inodes){
			pinode = list_entry(linode, struct inode, i_sb_list);
			count ++;
			printk("%lu[",pinode->i_ino);
			
			/* 
			 *traverse the dentry 
			*/
			
			hlist_for_each(ldentry, &pinode->i_dentry){
				pdentry = hlist_entry(ldentry, struct dentry, d_u.d_alias);
				printk("%s",pdentry->d_name.name);
			}
			printk("]\t");
		}
		printk("\n");
	}
	
	spin_unlock((spinlock_t *)sb_lock);
	return 0;
}

static void __exit exit_printfs(void)
{
	printk("exit_printfs\n");
}

module_init(printfs_init);
module_exit(exit_printfs);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is printfs module");
MODULE_VERSION("Ver 0.1");
