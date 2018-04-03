#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "color_temp_table.h"

// static 
// function [y2] = cubSpline(x, y, x2)
// 
//   if (size(x) ~= size(y))
//     error ("x and y are different lengths");
//   end
//   if (size(x,1)  < size(x,2))
//     x = x';
//   end
//   n  = size(x,1);
//   
//   % Preprocessor to find derivatives at each x
//   % monotonic version suppresses overshoot/ringing
//     
//   d = zeros(n-1,1);
//   for k=1:(n-1)
//     d(k) = (y(k+1)-y(k));
//   end
//   
//   m = zeros(n,1);
//   m(1) = d(1);
//   for k=2:(n-1)
//     m(k) = (d(k)+d(k-1))/2;      
//   end
//   m(n) = d(n-1);
//   
//   %% cubic spline with x,y,m (x is value at y) (m is 1st deriv at y)
//   % p(t) = (2 * t^3 - 3 * t^2 + 1) * p(0) + (t^3 - 2*t^2 + t) * m(0) + (-2 * t^3 + 3 * t^2) * p(1) + (t^3 - t^2) * m(1)
//   
//   % p(t) = [p(0) m(0) p(1) m(1)] [ 2 -3  0  1 ] [t^3]
//   %                              [ 1 -2  1  0 ] [t^2]
//   %                              [-2  3  0  0 ] [t^1]
//   %                              [ 1 -1  0  0 ] [t^0]
// 
//   cubS = [ ...
//      2 -3 0 1
//      1 -2 1 0
//     -2  3 0 0
//      1 -1 0 0
//   ];
// 
//   y2 = zeros(length(x2),1);
//   for i=1:length(x2)
//     ger = 1 + length(x) - find(x2(i) >= flipud(x), 1);
//     ler = find(x2(i) <= x, 1);
//     % printf("x2(i): %4f\nx (0): %4d\nx (1): %4d\n\n", x2(i), x(ge), x(le));
// 
//     if (ger == ler)
//       y2(i) = y(ger);
//     else
//       dt = (x2(i) - x(ger)) / (x(ger+1) - x(ger));
//       y2(i) = [y(ger) m(ger) y(ler) m(ler)] * cubS * [dt^3 dt^2 dt^1 dt^0]';
//     end
//   end
// end

#define MIN(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    _x < _y ? _x : _y; })

#define MAX(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    _x > _y ? _x : _y; })

int
interp_color_temp(
    color_temp_t *out
)
{
  //---------------
  // initialization
  static bool initialized = false;
  static color_temp_entry_t m[COLOR_TEMP_TABLE_LEN];
  if (!initialized) {
    // d
    int ii;
    color_temp_entry_t d[COLOR_TEMP_TABLE_LEN-1];
    for (ii=0; ii<COLOR_TEMP_TABLE_LEN-1; ii++) {
      d[ii].intensity  = color_temp_table[ii+1].intensity  - color_temp_table[ii].intensity;
      d[ii].norm_red   = color_temp_table[ii+1].norm_red   - color_temp_table[ii].norm_red;
      d[ii].norm_green = color_temp_table[ii+1].norm_green - color_temp_table[ii].norm_green;
      d[ii].norm_blue  = color_temp_table[ii+1].norm_blue  - color_temp_table[ii].norm_blue;
      d[ii].byte_red   = color_temp_table[ii+1].byte_red   - color_temp_table[ii].byte_red;
      d[ii].byte_green = color_temp_table[ii+1].byte_green - color_temp_table[ii].byte_green;
      d[ii].byte_blue  = color_temp_table[ii+1].byte_blue  - color_temp_table[ii].byte_blue;
    }

    // m
    ii=0;
    m[ii].intensity  = d[ii].intensity;
    m[ii].norm_red   = d[ii].norm_red;
    m[ii].norm_green = d[ii].norm_green;
    m[ii].norm_blue  = d[ii].norm_blue;
    m[ii].byte_red   = d[ii].byte_red;
    m[ii].byte_green = d[ii].byte_green;
    m[ii].byte_blue  = d[ii].byte_blue;
    for (ii=1; ii<COLOR_TEMP_TABLE_LEN-1; ii++) {
      m[ii].intensity  = (d[ii].intensity  + d[ii-1].intensity ) / 2;
      m[ii].norm_red   = (d[ii].norm_red   + d[ii-1].norm_red  ) / 2;
      m[ii].norm_green = (d[ii].norm_green + d[ii-1].norm_green) / 2;
      m[ii].norm_blue  = (d[ii].norm_blue  + d[ii-1].norm_blue ) / 2;
      m[ii].byte_red   = (d[ii].byte_red   + d[ii-1].byte_red  ) / 2;
      m[ii].byte_green = (d[ii].byte_green + d[ii-1].byte_green) / 2;
      m[ii].byte_blue  = (d[ii].byte_blue  + d[ii-1].byte_blue ) / 2;
    }
    m[ii].intensity  = d[ii].intensity;
    m[ii].norm_red   = d[ii].norm_red;
    m[ii].norm_green = d[ii].norm_green;
    m[ii].norm_blue  = d[ii].norm_blue;
    m[ii].byte_red   = d[ii].byte_red;
    m[ii].byte_green = d[ii].byte_green;
    m[ii].byte_blue  = d[ii].byte_blue;
  }

  //---------------
  // range check
  if (out->kelvin < color_temp_table[0].kelvin) {
    fprintf(stderr, "under range request. kelvin: %f\n", out->kelvin);

    out->kelvin     = color_temp_table[0].kelvin;
    out->intensity  = color_temp_table[0].intensity;
    out->norm_red   = color_temp_table[0].norm_red;
    out->norm_green = color_temp_table[0].norm_green;
    out->norm_blue  = color_temp_table[0].norm_blue;
    out->byte_red   = (int)nearbyint(color_temp_table[0].byte_red  );
    out->byte_green = (int)nearbyint(color_temp_table[0].byte_green);
    out->byte_blue  = (int)nearbyint(color_temp_table[0].byte_blue );

    fprintf(stderr, "returning data for kelvin == %f\n", out->kelvin);
    return -1;
  }
  if (out->kelvin > color_temp_table[COLOR_TEMP_TABLE_LEN-1].kelvin) {
    fprintf(stderr, "over range request. kelvin: %f\n", out->kelvin);

    out->kelvin     = color_temp_table[COLOR_TEMP_TABLE_LEN-1].kelvin;
    out->intensity  = color_temp_table[COLOR_TEMP_TABLE_LEN-1].intensity;
    out->norm_red   = color_temp_table[COLOR_TEMP_TABLE_LEN-1].norm_red;
    out->norm_green = color_temp_table[COLOR_TEMP_TABLE_LEN-1].norm_green;
    out->norm_blue  = color_temp_table[COLOR_TEMP_TABLE_LEN-1].norm_blue;
    out->byte_red   = (int)nearbyint(color_temp_table[COLOR_TEMP_TABLE_LEN-1].byte_red  );
    out->byte_green = (int)nearbyint(color_temp_table[COLOR_TEMP_TABLE_LEN-1].byte_green);
    out->byte_blue  = (int)nearbyint(color_temp_table[COLOR_TEMP_TABLE_LEN-1].byte_blue );

    fprintf(stderr, "returning data for kelvin == %f\n", out->kelvin);
    return -2;
  }

  //---------------
  // build a spline 
  double x = (out->kelvin / 100) - 10;
  int    n = (int)floor(x);
  double u[4];
  u[0] = 1;
  u[1] = x - n;
  u[2] = u[1] * u[1];
  u[3] = u[2] * u[1];

  double cubS_u[16] = {
    +2*u[3] -3*u[2] +0*u[1] +1*u[0],
    +1*u[3] -2*u[2] +1*u[1] +0*u[0],
    -2*u[3] +3*u[2] +0*u[1] +0*u[0],
    +1*u[3] -1*u[2] +0*u[1] +0*u[0]
  };

  // interp = [y(n) m(n) y(n+1) m(n+1)] * cubS * [u[3] u[2] u[1] u[0]]';
  out->intensity  = color_temp_table[n].intensity  * cubS_u[0] + m[n].intensity  * cubS_u[1] + color_temp_table[n+1].intensity  * cubS_u[2] + m[n+1].intensity  * cubS_u[3];
  out->norm_red   = MIN(1, MAX(0, color_temp_table[n].norm_red   * cubS_u[0] + m[n].norm_red   * cubS_u[1] + color_temp_table[n+1].norm_red   * cubS_u[2] + m[n+1].norm_red   * cubS_u[3]));
  out->norm_green = MIN(1, MAX(0, color_temp_table[n].norm_green * cubS_u[0] + m[n].norm_green * cubS_u[1] + color_temp_table[n+1].norm_green * cubS_u[2] + m[n+1].norm_green * cubS_u[3]));
  out->norm_blue  = MIN(1, MAX(0, color_temp_table[n].norm_blue  * cubS_u[0] + m[n].norm_blue  * cubS_u[1] + color_temp_table[n+1].norm_blue  * cubS_u[2] + m[n+1].norm_blue  * cubS_u[3]));
  out->byte_red   = nearbyint(MIN(255, MAX(0, color_temp_table[n].byte_red   * cubS_u[0] + m[n].byte_red   * cubS_u[1] + color_temp_table[n+1].byte_red   * cubS_u[2] + m[n+1].byte_red   * cubS_u[3])));
  out->byte_green = nearbyint(MIN(255, MAX(0, color_temp_table[n].byte_green * cubS_u[0] + m[n].byte_green * cubS_u[1] + color_temp_table[n+1].byte_green * cubS_u[2] + m[n+1].byte_green * cubS_u[3])));
  out->byte_blue  = nearbyint(MIN(255, MAX(0, color_temp_table[n].byte_blue  * cubS_u[0] + m[n].byte_blue  * cubS_u[1] + color_temp_table[n+1].byte_blue  * cubS_u[2] + m[n+1].byte_blue  * cubS_u[3])));

  return 0;
}
