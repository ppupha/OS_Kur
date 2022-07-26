#define pr_fmt(fmt) "myfs: " fmt

#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "myfs.h"

/*
 * Iterate over the files contained in dir and commit them in ctx.
 * This function is called by the VFS while ctx->pos changes.
 * Return 0 on success.
 */
static int myfs_iterate(struct file *dir, struct dir_context *ctx)
{
    struct inode *inode = file_inode(dir);
    struct myfs_inode_info *ci = MYFS_INODE(inode);
    struct super_block *sb = inode->i_sb;
    struct buffer_head *bh = NULL;
    struct myfs_dir_block *dblock = NULL;
    struct myfs_file *f = NULL;
    int i;

    /* Check that dir is a directory */
    if (!S_ISDIR(inode->i_mode))
        return -ENOTDIR;

    /*
     * Check that ctx->pos is not bigger than what we can handle (including
     * . and ..)
     */
    if (ctx->pos > MYFS_MAX_SUBFILES + 2)
        return 0;

    /* Commit . and .. to ctx */
    if (!dir_emit_dots(dir, ctx))
        return 0;

    /* Read the directory index block on disk */
    bh = sb_bread(sb, ci->dir_block);
    if (!bh)
        return -EIO;
    dblock = (struct myfs_dir_block *) bh->b_data;

    /* Iterate over the index block and commit subfiles */
    for (i = ctx->pos - 2; i < MYFS_MAX_SUBFILES; i++) {
        f = &dblock->files[i];
        if (!f->inode)
            break;
        if (!dir_emit(ctx, f->filename, MYFS_FILENAME_LEN, f->inode,
                      DT_UNKNOWN))
            break;
        ctx->pos++;
    }

    brelse(bh);

    return 0;
}

const struct file_operations myfs_dir_ops = {
    .owner = THIS_MODULE,
    .iterate_shared = myfs_iterate,
};
