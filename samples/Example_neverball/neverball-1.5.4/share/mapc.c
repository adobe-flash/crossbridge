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

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vec3.h"
#include "solid.h"
#include "base_image.h"
#include "base_config.h"
#include "fs.h"
#include "common.h"

#define MAXSTR 256
#define MAXKEY 16
#define SCALE  64.f
#define SMALL  0.0005f

/*
 * The overall design  of this map converter is  very stupid, but very
 * simple. It  begins by assuming  that every mtrl, vert,  edge, side,
 * and texc  in the map is  unique.  It then makes  an optimizing pass
 * that discards redundant information.  The result is optimal, though
 * the process is terribly inefficient.
 */

/*---------------------------------------------------------------------------*/

static const char *input_file;
static int         debug_output = 0;

/*---------------------------------------------------------------------------*/

/* Ohhhh... arbitrary! */

#define MAXM    1024
#define MAXV    65536
#define MAXE    65536
#define MAXS    65536
#define MAXT    131072
#define MAXG    65536
#define MAXL    4096
#define MAXN    2048
#define MAXP    2048
#define MAXB    1024
#define MAXH    2048
#define MAXZ    1024
#define MAXJ    1024
#define MAXX    1024
#define MAXR    2048
#define MAXU    1024
#define MAXW    1024
#define MAXD    1024
#define MAXA    16384
#define MAXI    262144

static int overflow(const char *s)
{
    printf("%s overflow\n", s);
    exit(1);
    return 0;
}

static int incm(struct s_file *fp)
{
    return (fp->mc < MAXM) ? fp->mc++ : overflow("mtrl");
}

static int incv(struct s_file *fp)
{
    return (fp->vc < MAXV) ? fp->vc++ : overflow("vert");
}

static int ince(struct s_file *fp)
{
    return (fp->ec < MAXE) ? fp->ec++ : overflow("edge");
}

static int incs(struct s_file *fp)
{
    return (fp->sc < MAXS) ? fp->sc++ : overflow("side");
}

static int inct(struct s_file *fp)
{
    return (fp->tc < MAXT) ? fp->tc++ : overflow("texc");
}

static int incg(struct s_file *fp)
{
    return (fp->gc < MAXG) ? fp->gc++ : overflow("geom");
}

static int incl(struct s_file *fp)
{
    return (fp->lc < MAXL) ? fp->lc++ : overflow("lump");
}

static int incn(struct s_file *fp)
{
    return (fp->nc < MAXN) ? fp->nc++ : overflow("node");
}

static int incp(struct s_file *fp)
{
    return (fp->pc < MAXP) ? fp->pc++ : overflow("path");
}

static int incb(struct s_file *fp)
{
    return (fp->bc < MAXB) ? fp->bc++ : overflow("body");
}

static int inch(struct s_file *fp)
{
    return (fp->hc < MAXH) ? fp->hc++ : overflow("item");
}

static int incz(struct s_file *fp)
{
    return (fp->zc < MAXZ) ? fp->zc++ : overflow("goal");
}

static int incj(struct s_file *fp)
{
    return (fp->jc < MAXJ) ? fp->jc++ : overflow("jump");
}

static int incx(struct s_file *fp)
{
    return (fp->xc < MAXX) ? fp->xc++ : overflow("swch");
}

static int incr(struct s_file *fp)
{
    return (fp->rc < MAXR) ? fp->rc++ : overflow("bill");
}

static int incu(struct s_file *fp)
{
    return (fp->uc < MAXU) ? fp->uc++ : overflow("ball");
}

static int incw(struct s_file *fp)
{
    return (fp->wc < MAXW) ? fp->wc++ : overflow("view");
}

static int incd(struct s_file *fp)
{
    return (fp->dc < MAXD) ? fp->dc++ : overflow("dict");
}

static int inci(struct s_file *fp)
{
    return (fp->ic < MAXI) ? fp->ic++ : overflow("indx");
}

static void init_file(struct s_file *fp)
{
    fp->mc = 0;
    fp->vc = 0;
    fp->ec = 0;
    fp->sc = 0;
    fp->tc = 0;
    fp->gc = 0;
    fp->lc = 0;
    fp->nc = 0;
    fp->pc = 0;
    fp->bc = 0;
    fp->hc = 0;
    fp->zc = 0;
    fp->jc = 0;
    fp->xc = 0;
    fp->rc = 0;
    fp->uc = 0;
    fp->wc = 0;
    fp->dc = 0;
    fp->ac = 0;
    fp->ic = 0;

    fp->mv = (struct s_mtrl *) calloc(MAXM, sizeof (struct s_mtrl));
    fp->vv = (struct s_vert *) calloc(MAXV, sizeof (struct s_vert));
    fp->ev = (struct s_edge *) calloc(MAXE, sizeof (struct s_edge));
    fp->sv = (struct s_side *) calloc(MAXS, sizeof (struct s_side));
    fp->tv = (struct s_texc *) calloc(MAXT, sizeof (struct s_texc));
    fp->gv = (struct s_geom *) calloc(MAXG, sizeof (struct s_geom));
    fp->lv = (struct s_lump *) calloc(MAXL, sizeof (struct s_lump));
    fp->nv = (struct s_node *) calloc(MAXN, sizeof (struct s_node));
    fp->pv = (struct s_path *) calloc(MAXP, sizeof (struct s_path));
    fp->bv = (struct s_body *) calloc(MAXB, sizeof (struct s_body));
    fp->hv = (struct s_item *) calloc(MAXH, sizeof (struct s_item));
    fp->zv = (struct s_goal *) calloc(MAXZ, sizeof (struct s_goal));
    fp->jv = (struct s_jump *) calloc(MAXJ, sizeof (struct s_jump));
    fp->xv = (struct s_swch *) calloc(MAXX, sizeof (struct s_swch));
    fp->rv = (struct s_bill *) calloc(MAXR, sizeof (struct s_bill));
    fp->uv = (struct s_ball *) calloc(MAXU, sizeof (struct s_ball));
    fp->wv = (struct s_view *) calloc(MAXW, sizeof (struct s_view));
    fp->dv = (struct s_dict *) calloc(MAXD, sizeof (struct s_dict));
    fp->av = (char          *) calloc(MAXA, sizeof (char));
    fp->iv = (int           *) calloc(MAXI, sizeof (int));
}

/*---------------------------------------------------------------------------*/

/*
 * The following is a small  symbol table data structure.  Symbols and
 * their integer  values are collected  in symv and  valv.  References
 * and pointers  to their unsatisfied integer values  are collected in
 * refv and pntv.  The resolve procedure matches references to symbols
 * and fills waiting ints with the proper values.
 */

#define MAXSYM 2048

static char symv[MAXSYM][MAXSTR];
static int  valv[MAXSYM];

static char refv[MAXSYM][MAXSTR];
static int *pntv[MAXSYM];

static int  strc;
static int  refc;

static void make_sym(const char *s, int  v)
{
    strncpy(symv[strc], s, MAXSTR - 1);
    valv[strc] = v;
    strc++;
}

static void make_ref(const char *r, int *p)
{
    strncpy(refv[refc], r, MAXSTR - 1);
    pntv[refc] = p;
    refc++;
}

static void resolve(void)
{
    int i, j;

    for (i = 0; i < refc; i++)
        for (j = 0; j < strc; j++)
            if (strncmp(refv[i], symv[j], MAXSTR) == 0)
            {
                *(pntv[i]) = valv[j];
                break;
            }
}

/*---------------------------------------------------------------------------*/

/*
 * The following globals are used to cache target_positions.  They are
 * targeted by various entities and must be resolved in a second pass.
 */

static float targ_p [MAXW][3];
static int   targ_wi[MAXW];
static int   targ_ji[MAXW];
static int   targ_n;

static void targets(struct s_file *fp)
{
    int i;

    for (i = 0; i < fp->wc; i++)
        v_cpy(fp->wv[i].q, targ_p[targ_wi[i]]);

    for (i = 0; i < fp->jc; i++)
        v_cpy(fp->jv[i].q, targ_p[targ_ji[i]]);
}

/*---------------------------------------------------------------------------*/

/*
 * The following code caches  image sizes.  Textures are referenced by
 * name,  but  their  sizes   are  necessary  when  computing  texture
 * coordinates.  This code  allows each file to be  accessed only once
 * regardless of the number of surfaces referring to it.
 */

struct _imagedata
{
    char *s;
    int w, h;
};

static struct _imagedata *imagedata = NULL;
static int image_n = 0;
static int image_alloc = 0;

#define IMAGE_REALLOC 32

static void free_imagedata()
{
    int i;

    if (imagedata)
    {
        for (i = 0; i < image_n; i++)
            free(imagedata[i].s);
        free(imagedata);
    }

    image_n = image_alloc = 0;
}

static int size_load(const char *file, int *w, int *h)
{
    void *p;

    if ((p = image_load(file, w, h, NULL)))
    {
        free(p);
        return 1;
    }
    return 0;
}

static void size_image(const char *name, int *w, int *h)
{
    char jpg[MAXSTR];
    char png[MAXSTR];
    int i;

    if (imagedata)
        for (i = 0; i < image_n; i++)
            if (strncmp(imagedata[i].s, name, MAXSTR) == 0)
            {
                *w = imagedata[i].w;
                *h = imagedata[i].h;

                return;
            }

    *w = 0;
    *h = 0;

    strcpy(jpg, name); strcat(jpg, ".jpg");
    strcpy(png, name); strcat(png, ".png");

    if (size_load(png, w, h) ||
        size_load(jpg, w, h))
    {

        if (image_n + 1 >= image_alloc)
        {
            struct _imagedata *tmp =
                (struct _imagedata *) malloc(sizeof(struct _imagedata) * (image_alloc + IMAGE_REALLOC));
            if (!tmp)
            {
                printf("malloc error\n");
                exit(1);
            }
            if (imagedata)
            {
                (void) memcpy(tmp, imagedata, sizeof(struct _imagedata) * image_alloc);
                free(imagedata);
            }
            imagedata = tmp;
            image_alloc += IMAGE_REALLOC;
        }

        imagedata[image_n].s = (char *) calloc(strlen(name) + 1, 1);
        imagedata[image_n].w = *w;
        imagedata[image_n].h = *h;
        strcpy(imagedata[image_n].s, name);

        image_n++;
    }
}

/*---------------------------------------------------------------------------*/

/* Read the given material file, adding a new material to the solid.  */

#define scan_vec4(f, s, v)                                              \
    if (fs_gets((s), sizeof (s), (f)))                                  \
        sscanf((s), "%f %f %f %f", (v), (v) + 1, (v) + 2, (v) + 3)

static int read_mtrl(struct s_file *fp, const char *name)
{
    static char line[MAXSTR];
    struct s_mtrl *mp;
    fs_file fin;
    int mi;

    for (mi = 0; mi < fp->mc; mi++)
        if (strncmp(name, fp->mv[mi].f, MAXSTR) == 0)
            return mi;

    mp = fp->mv + incm(fp);

    strncpy(mp->f, name, PATHMAX - 1);

    mp->a[0] = mp->a[1] = mp->a[2] = 0.2f;
    mp->d[0] = mp->d[1] = mp->d[2] = 0.8f;
    mp->s[0] = mp->s[1] = mp->s[2] = 0.0f;
    mp->e[0] = mp->e[1] = mp->e[2] = 0.0f;
    mp->a[3] = mp->d[3] = mp->s[3] = mp->e[3] = 1.0f;
    mp->h[0] = 0.0f;
    mp->fl   = 0;
    mp->angle = 45.0f;

    if ((fin = fs_open(name, "r")))
    {
        scan_vec4(fin, line, mp->d);
        scan_vec4(fin, line, mp->a);
        scan_vec4(fin, line, mp->s);
        scan_vec4(fin, line, mp->e);

        if (fs_gets(line, sizeof (line), fin))
            mp->h[0] = strtod(line, NULL);

        if (fs_gets(line, sizeof (line), fin))
            mp->fl = strtol(line, NULL, 10);

        if (fs_gets(line, sizeof (line), fin))
            mp->angle = strtod(line, NULL);

        fs_close(fin);
    }
    else
        fprintf(stderr, "%s: unknown material \"%s\"\n", input_file, name);

    return mi;
}

#undef scan_vec4

/*---------------------------------------------------------------------------*/

/*
 * All bodies with an associated  path are assumed to be positioned at
 * the  beginning of that  path.  These  bodies must  be moved  to the
 * origin  in order  for their  path transforms  to  behave correctly.
 * This is  how we get away  with defining func_trains  with no origin
 * specification.
 */

static void move_side(struct s_side *sp, const float p[3])
{
    sp->d -= v_dot(sp->n, p);
}

static void move_vert(struct s_vert *vp, const float p[3])
{
    v_sub(vp->p, vp->p, p);
}

static void move_lump(struct s_file *fp,
                      struct s_lump *lp, const float p[3])
{
    int i;

    for (i = 0; i < lp->sc; i++)
        move_side(fp->sv + fp->iv[lp->s0 + i], p);
    for (i = 0; i < lp->vc; i++)
        move_vert(fp->vv + fp->iv[lp->v0 + i], p);
}

static void move_body(struct s_file *fp,
                      struct s_body *bp)
{
    int i, *b;

    /* Move the lumps. */

    for (i = 0; i < bp->lc; i++)
        move_lump(fp, fp->lv + bp->l0 + i, fp->pv[bp->pi].p);

    /* Create an array to mark any verts referenced by moved geoms. */

    if (bp->gc > 0 && (b = (int *) calloc(fp->vc, sizeof (int))))
    {
        /* Mark the verts. */

        for (i = 0; i < bp->gc; i++)
        {
            b[fp->gv[fp->iv[bp->g0 + i]].vi] = 1;
            b[fp->gv[fp->iv[bp->g0 + i]].vj] = 1;
            b[fp->gv[fp->iv[bp->g0 + i]].vk] = 1;
        }

        /* Apply the motion to the marked vertices. */

        for (i = 0; i < fp->vc; ++i)
            if (b[i])
                move_vert(fp->vv + i, fp->pv[bp->pi].p);

        free(b);
    }
}

static void move_file(struct s_file *fp)
{
    int i;

    for (i = 0; i < fp->bc; i++)
        if (fp->bv[i].pi >= 0)
            move_body(fp, fp->bv + i);
}

/*---------------------------------------------------------------------------*/

/*
 * This is a basic OBJ loader.  It is by no means fully compliant with
 * the  OBJ  specification, but  it  works  well  with the  output  of
 * Wings3D.  All faces must be triangles and all vertices must include
 * normals and  texture coordinates.  Material  names are taken  to be
 * references to Neverball materials, rather than MTL definitions.
 */

static void read_vt(struct s_file *fp, const char *line)
{
    struct s_texc *tp = fp->tv + inct(fp);

    sscanf(line, "%f %f", tp->u, tp->u + 1);
}

static void read_vn(struct s_file *fp, const char *line)
{
    struct s_side *sp = fp->sv + incs(fp);

    sscanf(line, "%f %f %f", sp->n, sp->n + 1, sp->n + 2);
}

static void read_v(struct s_file *fp, const char *line)
{
    struct s_vert *vp = fp->vv + incv(fp);

    sscanf(line, "%f %f %f", vp->p, vp->p + 1, vp->p + 2);
}

static void read_f(struct s_file *fp, const char *line,
                   int v0, int t0, int s0, int mi)
{
    struct s_geom *gp = fp->gv + incg(fp);

    char c1;
    char c2;

    sscanf(line, "%d%c%d%c%d %d%c%d%c%d %d%c%d%c%d",
           &gp->vi, &c1, &gp->ti, &c2, &gp->si,
           &gp->vj, &c1, &gp->tj, &c2, &gp->sj,
           &gp->vk, &c1, &gp->tk, &c2, &gp->sk);

    gp->vi += (v0 - 1);
    gp->vj += (v0 - 1);
    gp->vk += (v0 - 1);
    gp->ti += (t0 - 1);
    gp->tj += (t0 - 1);
    gp->tk += (t0 - 1);
    gp->si += (s0 - 1);
    gp->sj += (s0 - 1);
    gp->sk += (s0 - 1);

    gp->mi  = mi;
}

static void read_obj(struct s_file *fp, const char *name, int mi)
{
    char line[MAXSTR];
    char mtrl[MAXSTR];
    fs_file fin;

    int v0 = fp->vc;
    int t0 = fp->tc;
    int s0 = fp->sc;

    if ((fin = fs_open(name, "r")))
    {
        while (fs_gets(line, MAXSTR, fin))
        {
            if (strncmp(line, "usemtl", 6) == 0)
            {
                sscanf(line + 6, "%s", mtrl);
                mi = read_mtrl(fp, mtrl);
            }

            else if (strncmp(line, "f", 1) == 0)
            {
                if (fp->mv[mi].d[3] > 0.0f)
                    read_f(fp, line + 1, v0, t0, s0, mi);
            }

            else if (strncmp(line, "vt", 2) == 0) read_vt(fp, line + 2);
            else if (strncmp(line, "vn", 2) == 0) read_vn(fp, line + 2);
            else if (strncmp(line, "v",  1) == 0) read_v (fp, line + 1);
        }
        fs_close(fin);
    }
}

/*---------------------------------------------------------------------------*/

static float plane_d[MAXS];
static float plane_n[MAXS][3];
static float plane_p[MAXS][3];
static float plane_u[MAXS][3];
static float plane_v[MAXS][3];
static int   plane_f[MAXS];
static int   plane_m[MAXS];

static void make_plane(int   pi, float x0, float y0, float      z0,
                       float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float tu, float tv, float r,
                       float su, float sv, int   fl, const char *s)
{
    static const float base[6][3][3] = {
        {{  0,  0,  1 }, {  1,  0,  0 }, {  0,  1,  0 }},
        {{  0,  0, -1 }, {  1,  0,  0 }, {  0,  1,  0 }},
        {{  1,  0,  0 }, {  0,  0, -1 }, {  0,  1,  0 }},
        {{ -1,  0,  0 }, {  0,  0, -1 }, {  0,  1,  0 }},
        {{  0,  1,  0 }, {  1,  0,  0 }, {  0,  0, -1 }},
        {{  0, -1,  0 }, {  1,  0,  0 }, {  0,  0, -1 }},
    };

    float R[16];
    float p0[3], p1[3], p2[3];
    float u[3],  v[3],  p[3];
    float k, d = 0.0f;
    int   i, n = 0;
    int   w, h;

    size_image(s, &w, &h);

    plane_f[pi] = fl ? L_DETAIL : 0;

    p0[0] = +x0 / SCALE;
    p0[1] = +z0 / SCALE;
    p0[2] = -y0 / SCALE;

    p1[0] = +x1 / SCALE;
    p1[1] = +z1 / SCALE;
    p1[2] = -y1 / SCALE;

    p2[0] = +x2 / SCALE;
    p2[1] = +z2 / SCALE;
    p2[2] = -y2 / SCALE;

    v_sub(u, p0, p1);
    v_sub(v, p2, p1);

    v_crs(plane_n[pi], u, v);
    v_nrm(plane_n[pi], plane_n[pi]);

    plane_d[pi] = v_dot(plane_n[pi], p1);

    for (i = 0; i < 6; i++)
        if ((k = v_dot(plane_n[pi], base[i][0])) >= d)
        {
            d = k;
            n = i;
        }

    p[0] = 0.f;
    p[1] = 0.f;
    p[2] = 0.f;

    /* Always rotate around the positive axis */

    m_rot(R, base[n - (n % 2)][0], V_RAD(r));

    v_mad(p, p, base[n][1], +su * tu / SCALE);
    v_mad(p, p, base[n][2], -sv * tv / SCALE);

    m_vxfm(plane_u[pi], R, base[n][1]);
    m_vxfm(plane_v[pi], R, base[n][2]);
    m_vxfm(plane_p[pi], R, p);

    v_scl(plane_u[pi], plane_u[pi], 64.f / w);
    v_scl(plane_v[pi], plane_v[pi], 64.f / h);

    v_scl(plane_u[pi], plane_u[pi], 1.f / su);
    v_scl(plane_v[pi], plane_v[pi], 1.f / sv);
}

/*---------------------------------------------------------------------------*/

#define T_EOF 0
#define T_BEG 1
#define T_CLP 2
#define T_KEY 3
#define T_END 4
#define T_NOP 5

static int map_token(fs_file fin, int pi, char key[MAXSTR], char val[MAXSTR])
{
    char buf[MAXSTR];

    if (fs_gets(buf, MAXSTR, fin))
    {
        char c;
        float x0, y0, z0;
        float x1, y1, z1;
        float x2, y2, z2;
        float tu, tv, r;
        float su, sv;
        int fl;

        /* Scan the beginning or end of a block. */

        if (buf[0] == '{') return T_BEG;
        if (buf[0] == '}') return T_END;

        /* Scan a key-value pair. */

        if (buf[0] == '\"')
        {
            strcpy(key, strtok(buf,  "\""));
            (void)      strtok(NULL, "\"");
            strcpy(val, strtok(NULL, "\""));

            return T_KEY;
        }

        /* Scan a plane. */

        if (sscanf(buf,
                   "%c %f %f %f %c "
                   "%c %f %f %f %c "
                   "%c %f %f %f %c "
                   "%s %f %f %f %f %f %d",
                   &c, &x0, &y0, &z0, &c,
                   &c, &x1, &y1, &z1, &c,
                   &c, &x2, &y2, &z2, &c,
                   key, &tu, &tv, &r, &su, &sv, &fl) == 22)
        {
            make_plane(pi, x0, y0, z0,
                       x1, y1, z1,
                       x2, y2, z2,
                       tu, tv, r, su, sv, fl, key);
            return T_CLP;
        }

        /* If it's not recognized, it must be uninteresting. */

        return T_NOP;
    }
    return T_EOF;
}

/*---------------------------------------------------------------------------*/

/* Parse a lump from the given file and add it to the solid. */

static void read_lump(struct s_file *fp, fs_file fin)
{
    char k[MAXSTR];
    char v[MAXSTR];
    int t;

    struct s_lump *lp = fp->lv + incl(fp);

    lp->s0 = fp->ic;

    while ((t = map_token(fin, fp->sc, k, v)))
    {
        if (t == T_CLP)
        {
            fp->sv[fp->sc].n[0] = plane_n[fp->sc][0];
            fp->sv[fp->sc].n[1] = plane_n[fp->sc][1];
            fp->sv[fp->sc].n[2] = plane_n[fp->sc][2];
            fp->sv[fp->sc].d    = plane_d[fp->sc];

            plane_m[fp->sc] = read_mtrl(fp, k);

            fp->iv[fp->ic] = fp->sc;
            inci(fp);
            incs(fp);
            lp->sc++;
        }
        if (t == T_END)
            break;
    }
}

/*---------------------------------------------------------------------------*/

static void make_path(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, pi = incp(fp);

    struct s_path *pp = fp->pv + pi;

    pp->p[0] = 0.f;
    pp->p[1] = 0.f;
    pp->p[2] = 0.f;
    pp->t    = 1.f;
    pp->pi   = pi;
    pp->f    = 1;
    pp->s    = 1;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "targetname") == 0)
            make_sym(v[i], pi);

        if (strcmp(k[i], "target") == 0)
            make_ref(v[i], &pp->pi);

        if (strcmp(k[i], "state") == 0)
            pp->f = atoi(v[i]);

        if (strcmp(k[i], "speed") == 0)
            sscanf(v[i], "%f", &pp->t);

        if (strcmp(k[i], "smooth") == 0)
            pp->s = atoi(v[i]);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            pp->p[0] = +x / SCALE;
            pp->p[1] = +z / SCALE;
            pp->p[2] = -y / SCALE;
        }
    }
}

static void make_dict(struct s_file *fp,
                      const char *k,
                      const char *v)
{
    int space_left, space_needed, di = incd(fp);

    struct s_dict *dp = fp->dv + di;

    space_left   = MAXA - fp->ac;
    space_needed = strlen(k) + 1 + strlen(v) + 1;

    if (space_needed > space_left)
    {
        fp->dc--;
        return;
    }

    dp->ai = fp->ac;
    dp->aj = dp->ai + strlen(k) + 1;
    fp->ac = dp->aj + strlen(v) + 1;

    strncpy(fp->av + dp->ai, k, space_left);
    strncpy(fp->av + dp->aj, v, space_left - strlen(k) - 1);
}

static int read_dict_entries = 0;

static void make_body(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c, int l0)
{
    int i, mi = 0, bi = incb(fp);

    int g0 = fp->gc;
    int v0 = fp->vc;

    float p[3];

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    struct s_body *bp = fp->bv + bi;

    bp->t  = 0.f;
    bp->pi = -1;
    bp->ni = -1;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "targetname") == 0)
            make_sym(v[i], bi);

        else if (strcmp(k[i], "target") == 0)
            make_ref(v[i], &bp->pi);

        else if (strcmp(k[i], "material") == 0)
            mi = read_mtrl(fp, v[i]);

        else if (strcmp(k[i], "model") == 0)
            read_obj(fp, v[i], mi);

        else if (strcmp(k[i], "origin") == 0)
            sscanf(v[i], "%f %f %f", &x, &y, &z);

        else if (read_dict_entries && strcmp(k[i], "classname") != 0)
            make_dict(fp, k[i], v[i]);
    }

    bp->l0 = l0;
    bp->lc = fp->lc - l0;
    bp->g0 = fp->ic;
    bp->gc = fp->gc - g0;

    for (i = 0; i < bp->gc; i++)
        fp->iv[inci(fp)] = g0++;

    p[0] = +x / SCALE;
    p[1] = +z / SCALE;
    p[2] = -y / SCALE;

    for (i = v0; i < fp->vc; i++)
        v_add(fp->vv[i].p, fp->vv[i].p, p);

    read_dict_entries = 0;
}

static void make_item(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, hi = inch(fp);

    struct s_item *hp = fp->hv + hi;

    hp->p[0] = 0.f;
    hp->p[1] = 0.f;
    hp->p[2] = 0.f;

    hp->t = ITEM_NONE;
    hp->n = 0;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "classname") == 0)
        {
            if (strcmp(v[i], "light") == 0)
                hp->t = ITEM_COIN;
            else if (strcmp(v[i], "item_health_large") == 0)
                hp->t = ITEM_GROW;
            else if (strcmp(v[i], "item_health_small") == 0)
                hp->t = ITEM_SHRINK;
        }

        if (strcmp(k[i], "light") == 0)
            sscanf(v[i], "%d", &hp->n);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            hp->p[0] = +x / SCALE;
            hp->p[1] = +z / SCALE;
            hp->p[2] = -y / SCALE;
        }
    }
}

static void make_bill(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, ri = incr(fp);

    struct s_bill *rp = fp->rv + ri;

    memset(rp, 0, sizeof (struct s_bill));
    rp->t = 1.0f;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "width") == 0)
            sscanf(v[i], "%f %f %f", rp->w, rp->w + 1, rp->w + 2);
        if (strcmp(k[i], "height") == 0)
            sscanf(v[i], "%f %f %f", rp->h, rp->h + 1, rp->h + 2);

        if (strcmp(k[i], "xrot") == 0)
            sscanf(v[i], "%f %f %f", rp->rx, rp->rx + 1, rp->rx + 2);
        if (strcmp(k[i], "yrot") == 0)
            sscanf(v[i], "%f %f %f", rp->ry, rp->ry + 1, rp->ry + 2);
        if (strcmp(k[i], "zrot") == 0)
            sscanf(v[i], "%f %f %f", rp->rz, rp->rz + 1, rp->rz + 2);

        if (strcmp(k[i], "time") == 0)
            sscanf(v[i], "%f", &rp->t);
        if (strcmp(k[i], "dist") == 0)
            sscanf(v[i], "%f", &rp->d);
        if (strcmp(k[i], "flag") == 0)
            sscanf(v[i], "%d", &rp->fl);

        if (strcmp(k[i], "image") == 0)
        {
            rp->mi = read_mtrl(fp, v[i]);
            fp->mv[rp->mi].fl |= M_CLAMPED;
        }

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            rp->p[0] = +x / SCALE;
            rp->p[1] = +z / SCALE;
            rp->p[2] = -y / SCALE;
        }
    }

    if (rp->fl & B_ADDITIVE)
        fp->mv[rp->mi].fl |= M_ADDITIVE;
}

static void make_goal(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, zi = incz(fp);

    struct s_goal *zp = fp->zv + zi;

    zp->p[0] = 0.f;
    zp->p[1] = 0.f;
    zp->p[2] = 0.f;
    zp->r    = 0.75;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "radius") == 0)
            sscanf(v[i], "%f", &zp->r);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            zp->p[0] = +(x)      / SCALE;
            zp->p[1] = +(z - 24) / SCALE;
            zp->p[2] = -(y)      / SCALE;
        }
    }
}

static void make_view(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, wi = incw(fp);

    struct s_view *wp = fp->wv + wi;

    wp->p[0] = 0.f;
    wp->p[1] = 0.f;
    wp->p[2] = 0.f;
    wp->q[0] = 0.f;
    wp->q[1] = 0.f;
    wp->q[2] = 0.f;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "target") == 0)
            make_ref(v[i], targ_wi + wi);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            wp->p[0] = +x / SCALE;
            wp->p[1] = +z / SCALE;
            wp->p[2] = -y / SCALE;
        }
    }
}

static void make_jump(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, ji = incj(fp);

    struct s_jump *jp = fp->jv + ji;

    jp->p[0] = 0.f;
    jp->p[1] = 0.f;
    jp->p[2] = 0.f;
    jp->q[0] = 0.f;
    jp->q[1] = 0.f;
    jp->q[2] = 0.f;
    jp->r    = 0.5;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "radius") == 0)
            sscanf(v[i], "%f", &jp->r);

        if (strcmp(k[i], "target") == 0)
            make_ref(v[i], targ_ji + ji);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            jp->p[0] = +x / SCALE;
            jp->p[1] = +z / SCALE;
            jp->p[2] = -y / SCALE;
        }
    }
}

static void make_swch(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, xi = incx(fp);

    struct s_swch *xp = fp->xv + xi;

    xp->p[0] = 0.f;
    xp->p[1] = 0.f;
    xp->p[2] = 0.f;
    xp->r    = 0.5;
    xp->pi   = 0;
    xp->t0   = 0;
    xp->t    = 0;
    xp->f0   = 0;
    xp->f    = 0;
    xp->i    = 0;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "radius") == 0)
            sscanf(v[i], "%f", &xp->r);

        if (strcmp(k[i], "target") == 0)
            make_ref(v[i], &xp->pi);

        if (strcmp(k[i], "timer") == 0)
        {
            sscanf(v[i], "%f", &xp->t0);
            xp->t = xp->t0;
        }

        if (strcmp(k[i], "state") == 0)
        {
            xp->f  = atoi(v[i]);
            xp->f0 = atoi(v[i]);
        }

        if (strcmp(k[i], "invisible") == 0)
            xp->i = atoi(v[i]);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            xp->p[0] = +x / SCALE;
            xp->p[1] = +z / SCALE;
            xp->p[2] = -y / SCALE;
        }
    }
}

static void make_targ(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i;

    targ_p[targ_n][0] = 0.f;
    targ_p[targ_n][1] = 0.f;
    targ_p[targ_n][2] = 0.f;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "targetname") == 0)
            make_sym(v[i], targ_n);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            targ_p[targ_n][0] = +x / SCALE;
            targ_p[targ_n][1] = +z / SCALE;
            targ_p[targ_n][2] = -y / SCALE;
        }
    }

    targ_n++;
}

static void make_ball(struct s_file *fp,
                      char k[][MAXSTR],
                      char v[][MAXSTR], int c)
{
    int i, ui = incu(fp);

    struct s_ball *up = fp->uv + ui;

    up->p[0] = 0.0f;
    up->p[1] = 0.0f;
    up->p[2] = 0.0f;
    up->r    = 0.25f;

    for (i = 0; i < c; i++)
    {
        if (strcmp(k[i], "radius") == 0)
            sscanf(v[i], "%f", &up->r);

        if (strcmp(k[i], "origin") == 0)
        {
            float x = 0.f, y = 0.f, z = 0.f;

            sscanf(v[i], "%f %f %f", &x, &y, &z);

            up->p[0] = +(x)      / SCALE;
            up->p[1] = +(z - 24) / SCALE;
            up->p[2] = -(y)      / SCALE;
        }
    }

    up->p[1] += up->r + SMALL;
}

/*---------------------------------------------------------------------------*/

static void read_ent(struct s_file *fp, fs_file fin)
{
    char k[MAXKEY][MAXSTR];
    char v[MAXKEY][MAXSTR];
    int t, i = 0, c = 0;

    int l0 = fp->lc;

    while ((t = map_token(fin, -1, k[c], v[c])))
    {
        if (t == T_KEY)
        {
            if (strcmp(k[c], "classname") == 0)
                i = c;
            c++;
        }
        if (t == T_BEG) read_lump(fp, fin);
        if (t == T_END) break;
    }

    if (!strcmp(v[i], "light"))                    make_item(fp, k, v, c);
    if (!strcmp(v[i], "item_health_large"))        make_item(fp, k, v, c);
    if (!strcmp(v[i], "item_health_small"))        make_item(fp, k, v, c);
    if (!strcmp(v[i], "info_camp"))                make_swch(fp, k, v, c);
    if (!strcmp(v[i], "info_null"))                make_bill(fp, k, v, c);
    if (!strcmp(v[i], "path_corner"))              make_path(fp, k, v, c);
    if (!strcmp(v[i], "info_player_start"))        make_ball(fp, k, v, c);
    if (!strcmp(v[i], "info_player_intermission")) make_view(fp, k, v, c);
    if (!strcmp(v[i], "info_player_deathmatch"))   make_goal(fp, k, v, c);
    if (!strcmp(v[i], "target_teleporter"))        make_jump(fp, k, v, c);
    if (!strcmp(v[i], "target_position"))          make_targ(fp, k, v, c);
    if (!strcmp(v[i], "worldspawn"))
    {
        read_dict_entries = 1;
        make_body(fp, k, v, c, l0);
    }
    if (!strcmp(v[i], "func_train"))               make_body(fp, k, v, c, l0);
    if (!strcmp(v[i], "misc_model"))               make_body(fp, k, v, c, l0);
}

static void read_map(struct s_file *fp, fs_file fin)
{
    char k[MAXSTR];
    char v[MAXSTR];
    int t;

    while ((t = map_token(fin, -1, k, v)))
        if (t == T_BEG)
            read_ent(fp, fin);
}

/*---------------------------------------------------------------------------*/

/* Test the location of a point with respect to a side plane. */

static int fore_side(const float p[3], const struct s_side *sp)
{
    return (v_dot(p, sp->n) - sp->d > +SMALL) ? 1 : 0;
}

static int on_side(const float p[3], const struct s_side *sp)
{
    float d = v_dot(p, sp->n) - sp->d;

    return (-SMALL < d && d < +SMALL) ? 1 : 0;
}

/*---------------------------------------------------------------------------*/
/*
 * Confirm  that  the addition  of  a vert  would  not  result in  degenerate
 * geometry.
 */

static int ok_vert(const struct s_file *fp,
                   const struct s_lump *lp, const float p[3])
{
    float r[3];
    int i;

    for (i = 0; i < lp->vc; i++)
    {
        float *q = fp->vv[fp->iv[lp->v0 + i]].p;

        v_sub(r, p, q);

        if (v_len(r) < SMALL)
            return 0;
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

/*
 * The following functions take the  set of planes defining a lump and
 * find the verts, edges, and  geoms that describe its boundaries.  To
 * do this, they first find the verts, and then search these verts for
 * valid edges and  geoms.  It may be more  efficient to compute edges
 * and  geoms directly  by clipping  down infinite  line  segments and
 * planes,  but this  would be  more  complex and  prone to  numerical
 * error.
 */

/*
 * Given 3  side planes,  compute the point  of intersection,  if any.
 * Confirm that this point falls  within the current lump, and that it
 * is unique.  Add it as a vert of the solid.
 */
static void clip_vert(struct s_file *fp,
                      struct s_lump *lp, int si, int sj, int sk)
{
    float M[16], X[16], I[16];
    float d[3],  p[3];
    int i;

    d[0] = fp->sv[si].d;
    d[1] = fp->sv[sj].d;
    d[2] = fp->sv[sk].d;

    m_basis(M, fp->sv[si].n, fp->sv[sj].n, fp->sv[sk].n);
    m_xps(X, M);

    if (m_inv(I, X))
    {
        m_vxfm(p, I, d);

        for (i = 0; i < lp->sc; i++)
        {
            int si = fp->iv[lp->s0 + i];

            if (fore_side(p, fp->sv + si))
                return;
        }

        if (ok_vert(fp, lp, p))
        {
            v_cpy(fp->vv[fp->vc].p, p);

            fp->iv[fp->ic] = fp->vc;
            inci(fp);
            incv(fp);
            lp->vc++;
        }
    }
}

/*
 * Given two  side planes,  find an edge  along their  intersection by
 * finding a pair of vertices that fall on both planes.  Add it to the
 * solid.
 */
static void clip_edge(struct s_file *fp,
                      struct s_lump *lp, int si, int sj)
{
    int i, j;

    for (i = 1; i < lp->vc; i++)
    {
        int vi = fp->iv[lp->v0 + i];

        if (!on_side(fp->vv[vi].p, fp->sv + si) ||
            !on_side(fp->vv[vi].p, fp->sv + sj))
            continue;

        for (j = 0; j < i; j++)
        {
            int vj = fp->iv[lp->v0 + j];

            if (on_side(fp->vv[vj].p, fp->sv + si) &&
                on_side(fp->vv[vj].p, fp->sv + sj))
            {
                fp->ev[fp->ec].vi = vi;
                fp->ev[fp->ec].vj = vj;

                fp->iv[fp->ic] = fp->ec;

                inci(fp);
                ince(fp);
                lp->ec++;
            }
        }
    }
}

/*
 * Find all verts that lie on  the given side of the lump.  Sort these
 * verts to  have a counter-clockwise winding about  the plane normal.
 * Create geoms to tessellate the resulting convex polygon.
 */
static void clip_geom(struct s_file *fp,
                      struct s_lump *lp, int si)
{
    int   m[256], t[256], d, i, j, n = 0;
    float u[3];
    float v[3];
    float w[3];

    struct s_side *sp = fp->sv + si;

    /* Find em. */

    for (i = 0; i < lp->vc; i++)
    {
        int vi = fp->iv[lp->v0 + i];

        if (on_side(fp->vv[vi].p, sp))
        {
            m[n] = vi;
            t[n] = inct(fp);

            v_add(v, fp->vv[vi].p, plane_p[si]);

            fp->tv[t[n]].u[0] = v_dot(v, plane_u[si]);
            fp->tv[t[n]].u[1] = v_dot(v, plane_v[si]);

            n++;
        }
    }

    /* Sort em. */

    for (i = 1; i < n; i++)
        for (j = i + 1; j < n; j++)
        {
            v_sub(u, fp->vv[m[i]].p, fp->vv[m[0]].p);
            v_sub(v, fp->vv[m[j]].p, fp->vv[m[0]].p);
            v_crs(w, u, v);

            if (v_dot(w, sp->n) < 0.f)
            {
                d     = m[i];
                m[i]  = m[j];
                m[j]  =    d;

                d     = t[i];
                t[i]  = t[j];
                t[j]  =    d;
            }
        }

    /* Index em. */

    for (i = 0; i < n - 2; i++)
    {
        fp->gv[fp->gc].mi = plane_m[si];

        fp->gv[fp->gc].ti = t[0];
        fp->gv[fp->gc].tj = t[i + 1];
        fp->gv[fp->gc].tk = t[i + 2];

        fp->gv[fp->gc].si = si;
        fp->gv[fp->gc].sj = si;
        fp->gv[fp->gc].sk = si;

        fp->gv[fp->gc].vi = m[0];
        fp->gv[fp->gc].vj = m[i + 1];
        fp->gv[fp->gc].vk = m[i + 2];

        fp->iv[fp->ic] = fp->gc;
        inci(fp);
        incg(fp);
        lp->gc++;
    }
}

/*
 * Iterate the sides of the lump, attempting to generate a new vert for
 * each trio of planes, a new edge  for each pair of planes, and a new
 * set of geom for each visible plane.
 */
static void clip_lump(struct s_file *fp, struct s_lump *lp)
{
    int i, j, k;

    lp->v0 = fp->ic;
    lp->vc = 0;

    for (i = 2; i < lp->sc; i++)
        for (j = 1; j < i; j++)
            for (k = 0; k < j; k++)
                clip_vert(fp, lp,
                          fp->iv[lp->s0 + i],
                          fp->iv[lp->s0 + j],
                          fp->iv[lp->s0 + k]);

    lp->e0 = fp->ic;
    lp->ec = 0;

    for (i = 1; i < lp->sc; i++)
        for (j = 0; j < i; j++)
            clip_edge(fp, lp,
                      fp->iv[lp->s0 + i],
                      fp->iv[lp->s0 + j]);

    lp->g0 = fp->ic;
    lp->gc = 0;

    for (i = 0; i < lp->sc; i++)
        if (fp->mv[plane_m[fp->iv[lp->s0 + i]]].d[3] > 0.0f)
            clip_geom(fp, lp,
                      fp->iv[lp->s0 + i]);

    for (i = 0; i < lp->sc; i++)
        if (plane_f[fp->iv[lp->s0 + i]])
            lp->fl |= L_DETAIL;
}

static void clip_file(struct s_file *fp)
{
    int i;

    for (i = 0; i < fp->lc; i++)
        clip_lump(fp, fp->lv + i);
}

/*---------------------------------------------------------------------------*/

/*
 * For each body element type,  determine if element 'p' is equivalent
 * to element  'q'.  This  is more than  a simple memory  compare.  It
 * effectively  snaps mtrls and  verts together,  and may  reverse the
 * winding of  an edge or a geom.   This is done in  order to maximize
 * the number of elements that can be eliminated.
 */

static int comp_mtrl(const struct s_mtrl *mp, const struct s_mtrl *mq)
{
    if (fabs(mp->d[0] - mq->d[0]) > SMALL) return 0;
    if (fabs(mp->d[1] - mq->d[1]) > SMALL) return 0;
    if (fabs(mp->d[2] - mq->d[2]) > SMALL) return 0;
    if (fabs(mp->d[3] - mq->d[3]) > SMALL) return 0;

    if (fabs(mp->a[0] - mq->a[0]) > SMALL) return 0;
    if (fabs(mp->a[1] - mq->a[1]) > SMALL) return 0;
    if (fabs(mp->a[2] - mq->a[2]) > SMALL) return 0;
    if (fabs(mp->a[3] - mq->a[3]) > SMALL) return 0;

    if (fabs(mp->s[0] - mq->s[0]) > SMALL) return 0;
    if (fabs(mp->s[1] - mq->s[1]) > SMALL) return 0;
    if (fabs(mp->s[2] - mq->s[2]) > SMALL) return 0;
    if (fabs(mp->s[3] - mq->s[3]) > SMALL) return 0;

    if (fabs(mp->e[0] - mq->e[0]) > SMALL) return 0;
    if (fabs(mp->e[1] - mq->e[1]) > SMALL) return 0;
    if (fabs(mp->e[2] - mq->e[2]) > SMALL) return 0;
    if (fabs(mp->e[3] - mq->e[3]) > SMALL) return 0;

    if (fabs(mp->h[0] - mq->h[0]) > SMALL) return 0;

    if (strncmp(mp->f, mq->f, PATHMAX)) return 0;

    return 1;
}

static int comp_vert(const struct s_vert *vp, const struct s_vert *vq)
{
    if (fabs(vp->p[0] - vq->p[0]) > SMALL) return 0;
    if (fabs(vp->p[1] - vq->p[1]) > SMALL) return 0;
    if (fabs(vp->p[2] - vq->p[2]) > SMALL) return 0;

    return 1;
}

static int comp_edge(const struct s_edge *ep, const struct s_edge *eq)
{
    if (ep->vi != eq->vi && ep->vi != eq->vj) return 0;
    if (ep->vj != eq->vi && ep->vj != eq->vj) return 0;

    return 1;
}

static int comp_side(const struct s_side *sp, const struct s_side *sq)
{
    if  (fabs(sp->d - sq->d) > SMALL)  return 0;
    if (v_dot(sp->n,  sq->n) < 0.9999) return 0;

    return 1;
}

static int comp_texc(const struct s_texc *tp, const struct s_texc *tq)
{
    if (fabs(tp->u[0] - tq->u[0]) > SMALL) return 0;
    if (fabs(tp->u[1] - tq->u[1]) > SMALL) return 0;

    return 1;
}

static int comp_geom(const struct s_geom *gp, const struct s_geom *gq)
{
    if (gp->mi != gq->mi) return 0;

    if (gp->ti != gq->ti) return 0;
    if (gp->si != gq->si) return 0;
    if (gp->vi != gq->vi) return 0;

    if (gp->tj != gq->tj) return 0;
    if (gp->sj != gq->sj) return 0;
    if (gp->vj != gq->vj) return 0;

    if (gp->tk != gq->tk) return 0;
    if (gp->sk != gq->sk) return 0;
    if (gp->vk != gq->vk) return 0;

    return 1;
}

/*---------------------------------------------------------------------------*/

/*
 * For each file  element type, replace all references  to element 'i'
 * with a  reference to element  'j'.  These are used  when optimizing
 * and sorting the file.
 */

static void swap_mtrl(struct s_file *fp, int mi, int mj)
{
    int i;

    for (i = 0; i < fp->gc; i++)
        if (fp->gv[i].mi == mi) fp->gv[i].mi = mj;
    for (i = 0; i < fp->rc; i++)
        if (fp->rv[i].mi == mi) fp->rv[i].mi = mj;
}

static int vert_swaps[MAXV];

static void apply_vert_swaps(struct s_file *fp)
{
    int i, j;

    for (i = 0; i < fp->ec; i++)
    {
        fp->ev[i].vi = vert_swaps[fp->ev[i].vi];
        fp->ev[i].vj = vert_swaps[fp->ev[i].vj];
    }

    for (i = 0; i < fp->gc; i++)
    {
        fp->gv[i].vi = vert_swaps[fp->gv[i].vi];
        fp->gv[i].vj = vert_swaps[fp->gv[i].vj];
        fp->gv[i].vk = vert_swaps[fp->gv[i].vk];
    }

    for (i = 0; i < fp->lc; i++)
        for (j = 0; j < fp->lv[i].vc; j++)
            fp->iv[fp->lv[i].v0 + j] = vert_swaps[fp->iv[fp->lv[i].v0 + j]];
}

static void swap_vert(struct s_file *fp, int vi, int vj)
{
    int i, j;

    for (i = 0; i < fp->ec; i++)
    {
        if (fp->ev[i].vi == vi) fp->ev[i].vi = vj;
        if (fp->ev[i].vj == vi) fp->ev[i].vj = vj;
    }

    for (i = 0; i < fp->gc; i++)
    {
        if (fp->gv[i].vi == vi) fp->gv[i].vi = vj;
        if (fp->gv[i].vj == vi) fp->gv[i].vj = vj;
        if (fp->gv[i].vk == vi) fp->gv[i].vk = vj;
    }

    for (i = 0; i < fp->lc; i++)
        for (j = 0; j < fp->lv[i].vc; j++)
            if (fp->iv[fp->lv[i].v0 + j] == vi)
                fp->iv[fp->lv[i].v0 + j]  = vj;
}

static int edge_swaps[MAXE];

static void apply_edge_swaps(struct s_file *fp)
{
    int i, j;

    for (i = 0; i < fp->lc; i++)
        for (j = 0; j < fp->lv[i].ec; j++)
            fp->iv[fp->lv[i].e0 + j] = edge_swaps[fp->iv[fp->lv[i].e0 + j]];
}

static int side_swaps[MAXS];

static void apply_side_swaps(struct s_file *fp)
{
    int i, j;

    for (i = 0; i < fp->gc; i++)
    {
        fp->gv[i].si = side_swaps[fp->gv[i].si];
        fp->gv[i].sj = side_swaps[fp->gv[i].sj];
        fp->gv[i].sk = side_swaps[fp->gv[i].sk];
    }
    for (i = 0; i < fp->nc; i++)
        fp->nv[i].si = side_swaps[fp->nv[i].si];

    for (i = 0; i < fp->lc; i++)
        for (j = 0; j < fp->lv[i].sc; j++)
            fp->iv[fp->lv[i].s0 + j] = side_swaps[fp->iv[fp->lv[i].s0 + j]];
}

static int texc_swaps[MAXT];

static void apply_texc_swaps(struct s_file *fp)
{
    int i;

    for (i = 0; i < fp->gc; i++)
    {
        fp->gv[i].ti = texc_swaps[fp->gv[i].ti];
        fp->gv[i].tj = texc_swaps[fp->gv[i].tj];
        fp->gv[i].tk = texc_swaps[fp->gv[i].tk];
    }
}

static int geom_swaps[MAXG];

static void apply_geom_swaps(struct s_file *fp)
{
    int i, j;

    for (i = 0; i < fp->lc; i++)
        for (j = 0; j < fp->lv[i].gc; j++)
            fp->iv[fp->lv[i].g0 + j] = geom_swaps[fp->iv[fp->lv[i].g0 + j]];

    for (i = 0; i < fp->bc; i++)
        for (j = 0; j < fp->bv[i].gc; j++)
            fp->iv[fp->bv[i].g0 + j] = geom_swaps[fp->iv[fp->bv[i].g0 + j]];
}

/*---------------------------------------------------------------------------*/

static void uniq_mtrl(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < fp->mc; i++)
    {
        for (j = 0; j < k; j++)
            if (comp_mtrl(fp->mv + i, fp->mv + j))
            {
                swap_mtrl(fp, i, j);
                break;
            }

        if (j == k)
        {
            if (i != k)
            {
                fp->mv[k] = fp->mv[i];
                swap_mtrl(fp, i, k);
            }
            k++;
        }
    }

    fp->mc = k;
}

static void uniq_vert(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < fp->vc; i++)
    {
        for (j = 0; j < k; j++)
            if (comp_vert(fp->vv + i, fp->vv + j))
                break;

        vert_swaps[i] = j;

        if (j == k)
        {
            if (i != k)
                fp->vv[k] = fp->vv[i];
            k++;
        }
    }

    apply_vert_swaps(fp);

    fp->vc = k;
}

static void uniq_edge(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < fp->ec; i++)
    {
        for (j = 0; j < k; j++)
            if (comp_edge(fp->ev + i, fp->ev + j))
                break;

        edge_swaps[i] = j;

        if (j == k)
        {
            if (i != k)
                fp->ev[k] = fp->ev[i];
            k++;
        }
    }

    apply_edge_swaps(fp);

    fp->ec = k;
}

static int geomlist[MAXV];
static int nextgeom[MAXG];

static void uniq_geom(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < MAXV; i++)
        geomlist[i] = -1;

    for (i = 0; i < fp->gc; i++)
    {
        int key = fp->gv[i].vj;

        for (j = geomlist[key]; j != -1; j = nextgeom[j])
            if (comp_geom(fp->gv + i, fp->gv + j))
                goto found;

        fp->gv[k] = fp->gv[i];

        nextgeom[k] = geomlist[key];
        geomlist[key] = k;

        j = k;
        k++;

found:
        geom_swaps[i] = j;
    }

    apply_geom_swaps(fp);

    fp->gc = k;
}

static void uniq_texc(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < fp->tc; i++)
    {
        for (j = 0; j < k; j++)
            if (comp_texc(fp->tv + i, fp->tv + j))
                break;

        texc_swaps[i] = j;

        if (j == k)
        {
            if (i != k)
                fp->tv[k] = fp->tv[i];
            k++;
        }
    }

    apply_texc_swaps(fp);

    fp->tc = k;
}

static void uniq_side(struct s_file *fp)
{
    int i, j, k = 0;

    for (i = 0; i < fp->sc; i++)
    {
        for (j = 0; j < k; j++)
            if (comp_side(fp->sv + i, fp->sv + j))
                break;

        side_swaps[i] = j;

        if (j == k)
        {
            if (i != k)
                fp->sv[k] = fp->sv[i];
            k++;
        }
    }

    apply_side_swaps(fp);

    fp->sc = k;
}

static void uniq_file(struct s_file *fp)
{
    /* Debug mode skips optimization, producing oversized output files. */

    if (debug_output == 0)
    {
        uniq_mtrl(fp);
        uniq_vert(fp);
        uniq_edge(fp);
        uniq_side(fp);
        uniq_texc(fp);
        uniq_geom(fp);
    }
}

/*---------------------------------------------------------------------------*/

struct s_trip
{
    int vi;
    int mi;
    int si;
    int gi;
};

static int comp_trip(const void *p, const void *q)
{
    const struct s_trip *tp = (const struct s_trip *) p;
    const struct s_trip *tq = (const struct s_trip *) q;

    if (tp->vi < tq->vi) return -1;
    if (tp->vi > tq->vi) return +1;
    if (tp->mi < tq->mi) return -1;
    if (tp->mi > tq->mi) return +1;

    return 0;
}

static void smth_file(struct s_file *fp)
{
    struct s_trip temp, *T;

    if (debug_output == 0)
    {
        if ((T = (struct s_trip *) malloc(fp->gc * 3 * sizeof (struct s_trip))))
        {
            int gi, i, j, k, l, c = 0;

            /* Create a list of all non-faceted vertex triplets. */

            for (gi = 0; gi < fp->gc; ++gi)
            {
                struct s_geom *gp = fp->gv + gi;

                T[c].vi = gp->vi;
                T[c].mi = gp->mi;
                T[c].si = gp->si;
                T[c].gi = gi;
                c++;

                T[c].vi = gp->vj;
                T[c].mi = gp->mi;
                T[c].si = gp->sj;
                T[c].gi = gi;
                c++;

                T[c].vi = gp->vk;
                T[c].mi = gp->mi;
                T[c].si = gp->sk;
                T[c].gi = gi;
                c++;
            }

            /* Sort all triplets by vertex index and material. */

            qsort(T, c, sizeof (struct s_trip), comp_trip);

            /* For each set of triplets sharing vertex index and material... */

            for (i = 0; i < c; i = l)
            {
                int acc = 0;

                float N[3], angle = fp->mv[T[i].mi].angle;
                const float   *Ni = fp->sv[T[i].si].n;

                /* Sort the set by side similarity to the first. */

                for (j = i + 1; j < c && (T[j].vi == T[i].vi &&
                                          T[j].mi == T[i].mi); ++j)
                {
                    for (k = j + 1; k < c && (T[k].vi == T[i].vi &&
                                              T[k].mi == T[i].mi); ++k)
                    {
                        const float *Nj = fp->sv[T[j].si].n;
                        const float *Nk = fp->sv[T[k].si].n;

                        if (T[j].si != T[k].si && v_dot(Nk, Ni) > v_dot(Nj, Ni))
                        {
                            temp = T[k];
                            T[k] = T[j];
                            T[j] = temp;
                        }
                    }
                }

                /* Accumulate all similar side normals. */

                N[0] = Ni[0];
                N[1] = Ni[1];
                N[2] = Ni[2];

                for (l = i + 1; l < c && (T[l].vi == T[i].vi &&
                                          T[l].mi == T[i].mi); ++l)
                    if (T[l].si != T[i].si)
                    {
                        const float *Nl = fp->sv[T[l].si].n;

                        if (V_DEG(facosf(v_dot(Ni, Nl))) > angle)
                            break;

                        N[0] += Nl[0];
                        N[1] += Nl[1];
                        N[2] += Nl[2];

                        acc++;
                    }

                /* If at least two normals have been accumulated... */

                if (acc)
                {
                    /* Store the accumulated normal as a new side. */

                    int ss = incs(fp);

                    v_nrm(fp->sv[ss].n, N);
                    fp->sv[ss].d = 0.0f;

                    /* Assign the new normal to the merged triplets. */

                    for (j = i; j < l; ++j)
                        T[j].si = ss;
                }
            }

            /* Assign the remapped normals to the original geoms. */

            for (i = 0; i < c; ++i)
            {
                struct s_geom *gp = fp->gv + T[i].gi;

                if (gp->vi == T[i].vi) gp->si = T[i].si;
                if (gp->vj == T[i].vi) gp->sj = T[i].si;
                if (gp->vk == T[i].vi) gp->sk = T[i].si;
            }

            free(T);
        }

        uniq_side(fp);
    }
}


/*---------------------------------------------------------------------------*/

static void sort_file(struct s_file *fp)
{
    int i, j;

    /* Sort billboards by material within distance. */

    for (i = 0; i < fp->rc; i++)
        for (j = i + 1; j < fp->rc; j++)
            if ((fp->rv[j].d  > fp->rv[i].d) ||
                (fp->rv[j].d == fp->rv[i].d &&
                 fp->rv[j].mi > fp->rv[i].mi))
            {
                struct s_bill t;

                t         = fp->rv[i];
                fp->rv[i] = fp->rv[j];
                fp->rv[j] =         t;
            }

    /* Ensure the first vertex is the lowest. */

    for (i = 0; i < fp->vc; i++)
        if (fp->vv[0].p[1] > fp->vv[i].p[1])
        {
            struct s_vert t;

            t         = fp->vv[0];
            fp->vv[0] = fp->vv[i];
            fp->vv[i] =         t;

            swap_vert(fp,  0, -1);
            swap_vert(fp,  i,  0);
            swap_vert(fp, -1,  i);
        }
}

/*---------------------------------------------------------------------------*/

static int test_lump_side(const struct s_file *fp,
                          const struct s_lump *lp,
                          const struct s_side *sp,
                          float bsphere[4])
{
    int si;
    int vi;

    int f = 0;
    int b = 0;

    float d;

    if (!lp->vc)
        return 0;

    /* Check if the bounding sphere of the lump is completely on one side. */

    d = v_dot(bsphere, sp->n) - sp->d;

    if (fabs(d) > bsphere[3])
        return d > 0 ? 1 : -1;

    /* If the given side is part of the given lump, then the lump is behind. */

    for (si = 0; si < lp->sc; si++)
        if (fp->sv + fp->iv[lp->s0 + si] == sp)
            return -1;

    /* Check if each lump vertex is in front of, behind, on the side. */

    for (vi = 0; vi < lp->vc; vi++)
    {
        float d = v_dot(fp->vv[fp->iv[lp->v0 + vi]].p, sp->n) - sp->d;

        if (d > 0) f++;
        if (d < 0) b++;
    }

    /* If no verts are behind, the lump is in front, and vice versa. */

    if (f > 0 && b == 0) return +1;
    if (b > 0 && f == 0) return -1;

    /* Else, the lump crosses the side. */

    return 0;
}

static int node_node(struct s_file *fp, int l0, int lc, float bsphere[][4])
{
    if (lc < 8)
    {
        /* Base case.  Dump all given lumps into a leaf node. */

        fp->nv[fp->nc].si = -1;
        fp->nv[fp->nc].ni = -1;
        fp->nv[fp->nc].nj = -1;
        fp->nv[fp->nc].l0 = l0;
        fp->nv[fp->nc].lc = lc;

        return incn(fp);
    }
    else
    {
        int sj  = 0;
        int sjd = lc;
        int sjo = lc;
        int si;
        int li = 0, lic = 0;
        int lj = 0, ljc = 0;
        int lk = 0, lkc = 0;
        int i;

        /* Find the side that most evenly splits the given lumps. */

        for (si = 0; si < fp->sc; si++)
        {
            int o = 0;
            int d = 0;
            int k = 0;

            for (li = 0; li < lc; li++)
                if ((k = test_lump_side(fp,
                                        fp->lv + l0 + li,
                                        fp->sv + si,
                                        bsphere[l0 + li])))
                    d += k;
                else
                    o++;

            d = abs(d);

            if ((d < sjd) || (d == sjd && o < sjo))
            {
                sj  = si;
                sjd = d;
                sjo = o;
            }
        }

        /* Flag each lump with its position WRT the side. */

        for (li = 0; li < lc; li++)
            if (debug_output)
            {
                fp->lv[l0+li].fl = (fp->lv[l0+li].fl & 1) | 0x20;
            }
            else
            {
                switch (test_lump_side(fp,
                                       fp->lv + l0 + li,
                                       fp->sv + sj,
                                       bsphere[l0 + li]))
                {
                case +1:
                    fp->lv[l0+li].fl = (fp->lv[l0+li].fl & 1) | 0x10;
                    break;

                case  0:
                    fp->lv[l0+li].fl = (fp->lv[l0+li].fl & 1) | 0x20;
                    break;

                case -1:
                    fp->lv[l0+li].fl = (fp->lv[l0+li].fl & 1) | 0x40;
                    break;
                }
            }

        /* Sort all lumps in the range by their flag values. */

        for (li = 1; li < lc; li++)
            for (lj = 0; lj < li; lj++)
                if (fp->lv[l0 + li].fl < fp->lv[l0 + lj].fl)
                {
                    struct s_lump l;
                    float f;
                    int i;

                    for (i = 0; i < 4; i++)
                    {
                        f                   = bsphere[l0 + li][i];
                        bsphere[l0 + li][i] = bsphere[l0 + lj][i];
                        bsphere[l0 + lj][i] =                   f;
                    }

                    l               = fp->lv[l0 + li];
                    fp->lv[l0 + li] = fp->lv[l0 + lj];
                    fp->lv[l0 + lj] =               l;
                }

        /* Establish the in-front, on, and behind lump ranges. */

        li = lic = 0;
        lj = ljc = 0;
        lk = lkc = 0;

        for (i = lc - 1; i >= 0; i--)
            switch (fp->lv[l0 + i].fl & 0xf0)
            {
            case 0x10: li = l0 + i; lic++; break;
            case 0x20: lj = l0 + i; ljc++; break;
            case 0x40: lk = l0 + i; lkc++; break;
            }

        /* Add the lumps on the side to the node. */

        i = incn(fp);

        fp->nv[i].si = sj;
        fp->nv[i].ni = node_node(fp, li, lic, bsphere);

        fp->nv[i].nj = node_node(fp, lk, lkc, bsphere);
        fp->nv[i].l0 = lj;
        fp->nv[i].lc = ljc;

        return i;
    }
}

/*
 * Compute a bounding sphere for a lump (not optimal)
 */
static void lump_bounding_sphere(struct s_file *fp,
                                 struct s_lump *lp,
                                 float bsphere[4])
{
    float bbox[6];
    float r;
    int i;

    if (!lp->vc)
        return;

    bbox[0] = bbox[3] = fp->vv[fp->iv[lp->v0]].p[0];
    bbox[1] = bbox[4] = fp->vv[fp->iv[lp->v0]].p[1];
    bbox[2] = bbox[5] = fp->vv[fp->iv[lp->v0]].p[2];

    for (i = 1; i < lp->vc; i++)
    {
        struct s_vert *vp = fp->vv + fp->iv[lp->v0 + i];
        int j;

        for (j = 0; j < 3; j++)
            if (vp->p[j] < bbox[j])
                bbox[j] = vp->p[j];

        for (j = 0; j < 3; j++)
            if (vp->p[j] > bbox[j + 3])
                bbox[j + 3] = vp->p[j];
    }

    r = 0;

    for (i = 0; i < 3; i++)
    {
        bsphere[i] = (bbox[i] + bbox[i + 3]) / 2;
        r += (bsphere[i] - bbox[i]) * (bsphere[i] - bbox[i]);
    }

    bsphere[3] = fsqrtf(r);
}

static void node_file(struct s_file *fp)
{
    float bsphere[MAXL][4];
    int i;

    /* Compute a bounding sphere for each lump. */

    for (i = 0; i < fp->lc; i++)
        lump_bounding_sphere(fp, fp->lv + i, bsphere[i]);

    /* Sort the lumps of each body into BSP nodes. */

    for (i = 0; i < fp->bc; i++)
        fp->bv[i].ni = node_node(fp, fp->bv[i].l0, fp->bv[i].lc, bsphere);
}

/*---------------------------------------------------------------------------*/

static void dump_file(struct s_file *p, const char *name)
{
    /* FIXME:  Count visible geoms.
     *
     * I'm afraid items break this (not sure though) so leaving it out.
     */

#if 0
    int i, j;
#endif
    int i;
    int c = 0;
    int n = 0;
#if 0
    int m = p->rc + p->cc * 128 + (p->zc * p->jc + p->xc) * 32;
#endif

    /* Count the number of solid lumps. */

    for (i = 0; i < p->lc; i++)
        if ((p->lv[i].fl & 1) == 0)
            n++;

#if 0
    /* Count the number of visible geoms. */

    for (i = 0; i < p->bc; i++)
    {
        for (j = 0; j < p->bv[i].lc; j++)
            m += p->lv[p->bv[i].l0 + j].gc;
        m += p->bv[i].gc;
    }
#endif

    /* Count the total value of all coins. */

    for (i = 0; i < p->hc; i++)
        if (p->hv[i].t == ITEM_COIN)
            c += p->hv[i].n;

#if 0
    printf("%s (%d/%d/$%d)\n"
#endif
    printf("%s (%d/$%d)\n"
           "  mtrl  vert  edge  side  texc"
           "  geom  lump  path  node  body\n"
           "%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n"
           "  item  goal  view  jump  swch"
           "  bill  ball  char  dict  indx\n"
           "%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n",
#if 0
           name, n, m, c,
#endif
           name, n, c,
           p->mc, p->vc, p->ec, p->sc, p->tc,
           p->gc, p->lc, p->pc, p->nc, p->bc,
           p->hc, p->zc, p->wc, p->jc, p->xc,
           p->rc, p->uc, p->ac, p->dc, p->ic);
}

int main(int argc, char *argv[])
{
    char src[MAXSTR] = "";
    char dst[MAXSTR] = "";
    struct s_file f;
    fs_file fin;

    if (!fs_init(argv[0]))
    {
        fprintf(stderr, "Failure to initialize virtual file system: %s\n",
                fs_error());
        return 1;
    }

    if (argc > 2)
    {
        input_file = argv[1];

        if (argc > 3 && strcmp(argv[3], "--debug") == 0)
            debug_output = 1;

        strncpy(src, argv[1], MAXSTR - 1);
        strncpy(dst, argv[1], MAXSTR - 1);

        if (strcmp(dst + strlen(dst) - 4, ".map") == 0)
            strcpy(dst + strlen(dst) - 4, ".sol");
        else
            strcat(dst, ".sol");

        fs_add_path     (dir_name(src));
        fs_set_write_dir(dir_name(dst));

        if ((fin = fs_open(base_name(src, NULL), "r")))
        {
            if (!fs_add_path_with_archives(argv[2]))
            {
                fprintf(stderr, "Failure to establish data directory\n");
                fs_close(fin);
                fs_quit();
                return 1;
            }

            init_file(&f);
            read_map(&f, fin);

            resolve();
            targets(&f);

            clip_file(&f);
            move_file(&f);
            uniq_file(&f);
            smth_file(&f);
            sort_file(&f);
            node_file(&f);
            dump_file(&f, dst);

            sol_stor(&f, base_name(dst, NULL));

            fs_close(fin);

            free_imagedata();
        }
    }
    else fprintf(stderr, "Usage: %s <map> <data> [--debug]\n", argv[0]);

    return 0;
}

