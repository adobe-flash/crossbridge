#include "game_common.h"
#include "vec3.h"

const char *status_to_str(int s)
{
    switch (s)
    {
    case GAME_NONE:    return _("Aborted");
    case GAME_TIME:    return _("Time-out");
    case GAME_GOAL:    return _("Success");
    case GAME_FALL:    return _("Fall-out");
    default:           return _("Unknown");
    }
}

const char *view_to_str(int v)
{
    switch (v)
    {
    case VIEW_CHASE:   return _("Chase");
    case VIEW_LAZY:    return _("Lazy");
    case VIEW_MANUAL:  return _("Manual");
    case VIEW_TOPDOWN: return _("Top-Down");
    default:           return _("Unknown");
    }
}

/*---------------------------------------------------------------------------*/

void game_tilt_init(struct game_tilt *tilt)
{
    tilt->x[0] = 1.0f;
    tilt->x[1] = 0.0f;
    tilt->x[2] = 0.0f;

    tilt->rx = 0.0f;

    tilt->z[0] = 0.0f;
    tilt->z[1] = 0.0f;
    tilt->z[2] = 1.0f;

    tilt->rz = 0.0f;
}

/*
 * Compute appropriate tilt axes from the view basis.
 */
void game_tilt_axes(struct game_tilt *tilt, float view_e[3][3])
{
    const float Y[3] = { 0.0f, 1.0f, 0.0f };

    v_cpy(tilt->x, view_e[0]);
    v_cpy(tilt->z, view_e[2]);

    /* Handle possible top-down view. */

    if (fabsf(v_dot(view_e[1], Y)) < fabsf(v_dot(view_e[2], Y)))
        v_inv(tilt->z, view_e[1]);
}

void game_tilt_grav(float h[3], const float g[3], const struct game_tilt *tilt)
{
    float X[16];
    float Z[16];
    float M[16];

    /* Compute the gravity vector from the given world rotations. */

    m_rot (Z, tilt->z, V_RAD(tilt->rz));
    m_rot (X, tilt->x, V_RAD(tilt->rx));
    m_mult(M, Z, X);
    m_vxfm(h, M, g);
}

/*---------------------------------------------------------------------------*/
