/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solid.h"
#include "base_config.h"
#include "binary.h"
#include "fs.h"

#define MAGIC       0x4c4f53af
#define SOL_VERSION 6

/*---------------------------------------------------------------------------*/

static void sol_load_mtrl(fs_file fin, struct s_mtrl *mp)
{
    get_array(fin,  mp->d, 4);
    get_array(fin,  mp->a, 4);
    get_array(fin,  mp->s, 4);
    get_array(fin,  mp->e, 4);
    get_array(fin,  mp->h, 1);
    get_index(fin, &mp->fl);

    fs_read(mp->f, 1, PATHMAX, fin);
}

static void sol_load_vert(fs_file fin, struct s_vert *vp)
{
    get_array(fin,  vp->p, 3);
}

static void sol_load_edge(fs_file fin, struct s_edge *ep)
{
    get_index(fin, &ep->vi);
    get_index(fin, &ep->vj);
}

static void sol_load_side(fs_file fin, struct s_side *sp)
{
    get_array(fin,  sp->n, 3);
    get_float(fin, &sp->d);
}

static void sol_load_texc(fs_file fin, struct s_texc *tp)
{
    get_array(fin,  tp->u, 2);
}

static void sol_load_geom(fs_file fin, struct s_geom *gp)
{
    get_index(fin, &gp->mi);
    get_index(fin, &gp->ti);
    get_index(fin, &gp->si);
    get_index(fin, &gp->vi);
    get_index(fin, &gp->tj);
    get_index(fin, &gp->sj);
    get_index(fin, &gp->vj);
    get_index(fin, &gp->tk);
    get_index(fin, &gp->sk);
    get_index(fin, &gp->vk);
}

static void sol_load_lump(fs_file fin, struct s_lump *lp)
{
    get_index(fin, &lp->fl);
    get_index(fin, &lp->v0);
    get_index(fin, &lp->vc);
    get_index(fin, &lp->e0);
    get_index(fin, &lp->ec);
    get_index(fin, &lp->g0);
    get_index(fin, &lp->gc);
    get_index(fin, &lp->s0);
    get_index(fin, &lp->sc);
}

static void sol_load_node(fs_file fin, struct s_node *np)
{
    get_index(fin, &np->si);
    get_index(fin, &np->ni);
    get_index(fin, &np->nj);
    get_index(fin, &np->l0);
    get_index(fin, &np->lc);
}

static void sol_load_path(fs_file fin, struct s_path *pp)
{
    get_array(fin,  pp->p, 3);
    get_float(fin, &pp->t);
    get_index(fin, &pp->pi);
    get_index(fin, &pp->f);
    get_index(fin, &pp->s);
}

static void sol_load_body(fs_file fin, struct s_body *bp)
{
    get_index(fin, &bp->pi);
    get_index(fin, &bp->ni);
    get_index(fin, &bp->l0);
    get_index(fin, &bp->lc);
    get_index(fin, &bp->g0);
    get_index(fin, &bp->gc);
}

static void sol_load_item(fs_file fin, struct s_item *hp)
{
    get_array(fin,  hp->p, 3);
    get_index(fin, &hp->t);
    get_index(fin, &hp->n);
}

static void sol_load_goal(fs_file fin, struct s_goal *zp)
{
    get_array(fin,  zp->p, 3);
    get_float(fin, &zp->r);
}

static void sol_load_swch(fs_file fin, struct s_swch *xp)
{
    get_array(fin,  xp->p, 3);
    get_float(fin, &xp->r);
    get_index(fin, &xp->pi);
    get_float(fin, &xp->t0);
    get_float(fin, &xp->t);
    get_index(fin, &xp->f0);
    get_index(fin, &xp->f);
    get_index(fin, &xp->i);
}

static void sol_load_bill(fs_file fin, struct s_bill *rp)
{
    get_index(fin, &rp->fl);
    get_index(fin, &rp->mi);
    get_float(fin, &rp->t);
    get_float(fin, &rp->d);
    get_array(fin,  rp->w,  3);
    get_array(fin,  rp->h,  3);
    get_array(fin,  rp->rx, 3);
    get_array(fin,  rp->ry, 3);
    get_array(fin,  rp->rz, 3);
    get_array(fin,  rp->p,  3);
}

static void sol_load_jump(fs_file fin, struct s_jump *jp)
{
    get_array(fin,  jp->p, 3);
    get_array(fin,  jp->q, 3);
    get_float(fin, &jp->r);
}

static void sol_load_ball(fs_file fin, struct s_ball *bp)
{
    get_array(fin,  bp->p, 3);
    get_float(fin, &bp->r);

    bp->e[0][0] = bp->E[0][0] = 1.0f;
    bp->e[0][1] = bp->E[0][1] = 0.0f;
    bp->e[0][2] = bp->E[0][2] = 0.0f;

    bp->e[1][0] = bp->E[1][0] = 0.0f;
    bp->e[1][1] = bp->E[1][1] = 1.0f;
    bp->e[1][2] = bp->E[1][2] = 0.0f;

    bp->e[2][0] = bp->E[2][0] = 0.0f;
    bp->e[2][1] = bp->E[2][1] = 0.0f;
    bp->e[2][2] = bp->E[2][2] = 1.0f;
}

static void sol_load_view(fs_file fin, struct s_view *wp)
{
    get_array(fin,  wp->p, 3);
    get_array(fin,  wp->q, 3);
}

static void sol_load_dict(fs_file fin, struct s_dict *dp)
{
    get_index(fin, &dp->ai);
    get_index(fin, &dp->aj);
}

static int sol_load_file(fs_file fin, struct s_file *fp)
{
    int i;
    int magic;
    int version;

    get_index(fin, &magic);
    get_index(fin, &version);

    if (magic != MAGIC || version != SOL_VERSION)
        return 0;

    get_index(fin, &fp->ac);
    get_index(fin, &fp->dc);
    get_index(fin, &fp->mc);
    get_index(fin, &fp->vc);
    get_index(fin, &fp->ec);
    get_index(fin, &fp->sc);
    get_index(fin, &fp->tc);
    get_index(fin, &fp->gc);
    get_index(fin, &fp->lc);
    get_index(fin, &fp->nc);
    get_index(fin, &fp->pc);
    get_index(fin, &fp->bc);
    get_index(fin, &fp->hc);
    get_index(fin, &fp->zc);
    get_index(fin, &fp->jc);
    get_index(fin, &fp->xc);
    get_index(fin, &fp->rc);
    get_index(fin, &fp->uc);
    get_index(fin, &fp->wc);
    get_index(fin, &fp->ic);

    if (fp->ac)
        fp->av = (char          *) calloc(fp->ac, sizeof (char));
    if (fp->mc)
        fp->mv = (struct s_mtrl *) calloc(fp->mc, sizeof (struct s_mtrl));
    if (fp->vc)
        fp->vv = (struct s_vert *) calloc(fp->vc, sizeof (struct s_vert));
    if (fp->ec)
        fp->ev = (struct s_edge *) calloc(fp->ec, sizeof (struct s_edge));
    if (fp->sc)
        fp->sv = (struct s_side *) calloc(fp->sc, sizeof (struct s_side));
    if (fp->tc)
        fp->tv = (struct s_texc *) calloc(fp->tc, sizeof (struct s_texc));
    if (fp->gc)
        fp->gv = (struct s_geom *) calloc(fp->gc, sizeof (struct s_geom));
    if (fp->lc)
        fp->lv = (struct s_lump *) calloc(fp->lc, sizeof (struct s_lump));
    if (fp->nc)
        fp->nv = (struct s_node *) calloc(fp->nc, sizeof (struct s_node));
    if (fp->pc)
        fp->pv = (struct s_path *) calloc(fp->pc, sizeof (struct s_path));
    if (fp->bc)
        fp->bv = (struct s_body *) calloc(fp->bc, sizeof (struct s_body));
    if (fp->hc)
        fp->hv = (struct s_item *) calloc(fp->hc, sizeof (struct s_item));
    if (fp->zc)
        fp->zv = (struct s_goal *) calloc(fp->zc, sizeof (struct s_goal));
    if (fp->jc)
        fp->jv = (struct s_jump *) calloc(fp->jc, sizeof (struct s_jump));
    if (fp->xc)
        fp->xv = (struct s_swch *) calloc(fp->xc, sizeof (struct s_swch));
    if (fp->rc)
        fp->rv = (struct s_bill *) calloc(fp->rc, sizeof (struct s_bill));
    if (fp->uc)
        fp->uv = (struct s_ball *) calloc(fp->uc, sizeof (struct s_ball));
    if (fp->wc)
        fp->wv = (struct s_view *) calloc(fp->wc, sizeof (struct s_view));
    if (fp->dc)
        fp->dv = (struct s_dict *) calloc(fp->dc, sizeof (struct s_dict));
    if (fp->ic)
        fp->iv = (int           *) calloc(fp->ic, sizeof (int));

    if (fp->ac)
        fs_read(fp->av, 1, fp->ac, fin);

    for (i = 0; i < fp->dc; i++) sol_load_dict(fin, fp->dv + i);
    for (i = 0; i < fp->mc; i++) sol_load_mtrl(fin, fp->mv + i);
    for (i = 0; i < fp->vc; i++) sol_load_vert(fin, fp->vv + i);
    for (i = 0; i < fp->ec; i++) sol_load_edge(fin, fp->ev + i);
    for (i = 0; i < fp->sc; i++) sol_load_side(fin, fp->sv + i);
    for (i = 0; i < fp->tc; i++) sol_load_texc(fin, fp->tv + i);
    for (i = 0; i < fp->gc; i++) sol_load_geom(fin, fp->gv + i);
    for (i = 0; i < fp->lc; i++) sol_load_lump(fin, fp->lv + i);
    for (i = 0; i < fp->nc; i++) sol_load_node(fin, fp->nv + i);
    for (i = 0; i < fp->pc; i++) sol_load_path(fin, fp->pv + i);
    for (i = 0; i < fp->bc; i++) sol_load_body(fin, fp->bv + i);
    for (i = 0; i < fp->hc; i++) sol_load_item(fin, fp->hv + i);
    for (i = 0; i < fp->zc; i++) sol_load_goal(fin, fp->zv + i);
    for (i = 0; i < fp->jc; i++) sol_load_jump(fin, fp->jv + i);
    for (i = 0; i < fp->xc; i++) sol_load_swch(fin, fp->xv + i);
    for (i = 0; i < fp->rc; i++) sol_load_bill(fin, fp->rv + i);
    for (i = 0; i < fp->uc; i++) sol_load_ball(fin, fp->uv + i);
    for (i = 0; i < fp->wc; i++) sol_load_view(fin, fp->wv + i);
    for (i = 0; i < fp->ic; i++) get_index(fin, fp->iv + i);

    return 1;
}

static int sol_load_head(fs_file fin, struct s_file *fp)
{
    int magic;
    int version;

    get_index(fin, &magic);
    get_index(fin, &version);

    if (magic != MAGIC || version != SOL_VERSION)
        return 0;

    get_index(fin, &fp->ac);
    get_index(fin, &fp->dc);

#if 0
    get_index(fin, &fp->mc);
    get_index(fin, &fp->vc);
    get_index(fin, &fp->ec);
    get_index(fin, &fp->sc);
    get_index(fin, &fp->tc);
    get_index(fin, &fp->gc);
    get_index(fin, &fp->lc);
    get_index(fin, &fp->nc);
    get_index(fin, &fp->pc);
    get_index(fin, &fp->bc);
    get_index(fin, &fp->hc);
    get_index(fin, &fp->zc);
    get_index(fin, &fp->jc);
    get_index(fin, &fp->xc);
    get_index(fin, &fp->rc);
    get_index(fin, &fp->uc);
    get_index(fin, &fp->wc);
    get_index(fin, &fp->ic);
#endif
    fs_seek(fin, 18 * 4, SEEK_CUR);

    if (fp->ac)
    {
        fp->av = (char *) calloc(fp->ac, sizeof (char));
        fs_read(fp->av, 1, fp->ac, fin);
    }

    if (fp->dc)
    {
        int i;

        fp->dv = (struct s_dict *) calloc(fp->dc, sizeof (struct s_dict));

        for (i = 0; i < fp->dc; i++)
            sol_load_dict(fin, fp->dv + i);
    }

    return 1;
}

int sol_load_only_file(struct s_file *fp, const char *filename)
{
    fs_file fin;
    int res = 0;

    if ((fin = fs_open(filename, "r")))
    {
        res = sol_load_file(fin, fp);
        fs_close(fin);
    }
    return res;
}

int sol_load_only_head(struct s_file *fp, const char *filename)
{
    fs_file fin;
    int res = 0;

    if ((fin = fs_open(filename, "r")))
    {
        res = sol_load_head(fin, fp);
        fs_close(fin);
    }
    return res;
}

/*---------------------------------------------------------------------------*/

static void sol_stor_mtrl(fs_file fout, struct s_mtrl *mp)
{
    put_array(fout,  mp->d, 4);
    put_array(fout,  mp->a, 4);
    put_array(fout,  mp->s, 4);
    put_array(fout,  mp->e, 4);
    put_array(fout,  mp->h, 1);
    put_index(fout, &mp->fl);

    fs_write(mp->f, 1, PATHMAX, fout);
}

static void sol_stor_vert(fs_file fout, struct s_vert *vp)
{
    put_array(fout,  vp->p, 3);
}

static void sol_stor_edge(fs_file fout, struct s_edge *ep)
{
    put_index(fout, &ep->vi);
    put_index(fout, &ep->vj);
}

static void sol_stor_side(fs_file fout, struct s_side *sp)
{
    put_array(fout,  sp->n, 3);
    put_float(fout, &sp->d);
}

static void sol_stor_texc(fs_file fout, struct s_texc *tp)
{
    put_array(fout,  tp->u, 2);
}

static void sol_stor_geom(fs_file fout, struct s_geom *gp)
{
    put_index(fout, &gp->mi);
    put_index(fout, &gp->ti);
    put_index(fout, &gp->si);
    put_index(fout, &gp->vi);
    put_index(fout, &gp->tj);
    put_index(fout, &gp->sj);
    put_index(fout, &gp->vj);
    put_index(fout, &gp->tk);
    put_index(fout, &gp->sk);
    put_index(fout, &gp->vk);
}

static void sol_stor_lump(fs_file fout, struct s_lump *lp)
{
    put_index(fout, &lp->fl);
    put_index(fout, &lp->v0);
    put_index(fout, &lp->vc);
    put_index(fout, &lp->e0);
    put_index(fout, &lp->ec);
    put_index(fout, &lp->g0);
    put_index(fout, &lp->gc);
    put_index(fout, &lp->s0);
    put_index(fout, &lp->sc);
}

static void sol_stor_node(fs_file fout, struct s_node *np)
{
    put_index(fout, &np->si);
    put_index(fout, &np->ni);
    put_index(fout, &np->nj);
    put_index(fout, &np->l0);
    put_index(fout, &np->lc);
}

static void sol_stor_path(fs_file fout, struct s_path *pp)
{
    put_array(fout,  pp->p, 3);
    put_float(fout, &pp->t);
    put_index(fout, &pp->pi);
    put_index(fout, &pp->f);
    put_index(fout, &pp->s);
}

static void sol_stor_body(fs_file fout, struct s_body *bp)
{
    put_index(fout, &bp->pi);
    put_index(fout, &bp->ni);
    put_index(fout, &bp->l0);
    put_index(fout, &bp->lc);
    put_index(fout, &bp->g0);
    put_index(fout, &bp->gc);
}

static void sol_stor_item(fs_file fout, struct s_item *hp)
{
    put_array(fout,  hp->p, 3);
    put_index(fout, &hp->t);
    put_index(fout, &hp->n);
}

static void sol_stor_goal(fs_file fout, struct s_goal *zp)
{
    put_array(fout,  zp->p, 3);
    put_float(fout, &zp->r);
}

static void sol_stor_swch(fs_file fout, struct s_swch *xp)
{
    put_array(fout,  xp->p, 3);
    put_float(fout, &xp->r);
    put_index(fout, &xp->pi);
    put_float(fout, &xp->t0);
    put_float(fout, &xp->t);
    put_index(fout, &xp->f0);
    put_index(fout, &xp->f);
    put_index(fout, &xp->i);
}

static void sol_stor_bill(fs_file fout, struct s_bill *rp)
{
    put_index(fout, &rp->fl);
    put_index(fout, &rp->mi);
    put_float(fout, &rp->t);
    put_float(fout, &rp->d);
    put_array(fout,  rp->w,  3);
    put_array(fout,  rp->h,  3);
    put_array(fout,  rp->rx, 3);
    put_array(fout,  rp->ry, 3);
    put_array(fout,  rp->rz, 3);
    put_array(fout,  rp->p,  3);
}

static void sol_stor_jump(fs_file fout, struct s_jump *jp)
{
    put_array(fout,  jp->p, 3);
    put_array(fout,  jp->q, 3);
    put_float(fout, &jp->r);
}

static void sol_stor_ball(fs_file fout, struct s_ball *bp)
{
    put_array(fout,  bp->p, 3);
    put_float(fout, &bp->r);
}

static void sol_stor_view(fs_file fout, struct s_view *wp)
{
    put_array(fout,  wp->p, 3);
    put_array(fout,  wp->q, 3);
}

static void sol_stor_dict(fs_file fout, struct s_dict *dp)
{
    put_index(fout, &dp->ai);
    put_index(fout, &dp->aj);
}

static void sol_stor_file(fs_file fout, struct s_file *fp)
{
    int i;
    int magic   = MAGIC;
    int version = SOL_VERSION;

    put_index(fout, &magic);
    put_index(fout, &version);

    put_index(fout, &fp->ac);
    put_index(fout, &fp->dc);
    put_index(fout, &fp->mc);
    put_index(fout, &fp->vc);
    put_index(fout, &fp->ec);
    put_index(fout, &fp->sc);
    put_index(fout, &fp->tc);
    put_index(fout, &fp->gc);
    put_index(fout, &fp->lc);
    put_index(fout, &fp->nc);
    put_index(fout, &fp->pc);
    put_index(fout, &fp->bc);
    put_index(fout, &fp->hc);
    put_index(fout, &fp->zc);
    put_index(fout, &fp->jc);
    put_index(fout, &fp->xc);
    put_index(fout, &fp->rc);
    put_index(fout, &fp->uc);
    put_index(fout, &fp->wc);
    put_index(fout, &fp->ic);

    fs_write(fp->av, 1, fp->ac, fout);

    for (i = 0; i < fp->dc; i++) sol_stor_dict(fout, fp->dv + i);
    for (i = 0; i < fp->mc; i++) sol_stor_mtrl(fout, fp->mv + i);
    for (i = 0; i < fp->vc; i++) sol_stor_vert(fout, fp->vv + i);
    for (i = 0; i < fp->ec; i++) sol_stor_edge(fout, fp->ev + i);
    for (i = 0; i < fp->sc; i++) sol_stor_side(fout, fp->sv + i);
    for (i = 0; i < fp->tc; i++) sol_stor_texc(fout, fp->tv + i);
    for (i = 0; i < fp->gc; i++) sol_stor_geom(fout, fp->gv + i);
    for (i = 0; i < fp->lc; i++) sol_stor_lump(fout, fp->lv + i);
    for (i = 0; i < fp->nc; i++) sol_stor_node(fout, fp->nv + i);
    for (i = 0; i < fp->pc; i++) sol_stor_path(fout, fp->pv + i);
    for (i = 0; i < fp->bc; i++) sol_stor_body(fout, fp->bv + i);
    for (i = 0; i < fp->hc; i++) sol_stor_item(fout, fp->hv + i);
    for (i = 0; i < fp->zc; i++) sol_stor_goal(fout, fp->zv + i);
    for (i = 0; i < fp->jc; i++) sol_stor_jump(fout, fp->jv + i);
    for (i = 0; i < fp->xc; i++) sol_stor_swch(fout, fp->xv + i);
    for (i = 0; i < fp->rc; i++) sol_stor_bill(fout, fp->rv + i);
    for (i = 0; i < fp->uc; i++) sol_stor_ball(fout, fp->uv + i);
    for (i = 0; i < fp->wc; i++) sol_stor_view(fout, fp->wv + i);
    for (i = 0; i < fp->ic; i++) put_index(fout, fp->iv + i);
}

/*---------------------------------------------------------------------------*/

int sol_stor(struct s_file *fp, const char *filename)
{
    fs_file fout;

    if ((fout = fs_open(filename, "w")))
    {
        sol_stor_file(fout, fp);
        fs_close(fout);

        return 1;
    }
    return 0;
}

void sol_free(struct s_file *fp)
{
    if (fp->av) free(fp->av);
    if (fp->mv) free(fp->mv);
    if (fp->vv) free(fp->vv);
    if (fp->ev) free(fp->ev);
    if (fp->sv) free(fp->sv);
    if (fp->tv) free(fp->tv);
    if (fp->gv) free(fp->gv);
    if (fp->lv) free(fp->lv);
    if (fp->nv) free(fp->nv);
    if (fp->pv) free(fp->pv);
    if (fp->bv) free(fp->bv);
    if (fp->hv) free(fp->hv);
    if (fp->zv) free(fp->zv);
    if (fp->jv) free(fp->jv);
    if (fp->xv) free(fp->xv);
    if (fp->rv) free(fp->rv);
    if (fp->uv) free(fp->uv);
    if (fp->wv) free(fp->wv);
    if (fp->dv) free(fp->dv);
    if (fp->iv) free(fp->iv);

    memset(fp, 0, sizeof (struct s_file));
}

/*---------------------------------------------------------------------------*/
