/*
 * Copyright (C) 2016 Javad Doliskani, javad.doliskani@uwaterloo.ca
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ISOGENY_H
#define ISOGENY_H

#include "sidh_elliptic_curve.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Representation of an isogeny between two elliptic curve
 */
typedef struct {
    // Let the kernel K of the isogeny (excluding the zero point) be the union
    // of F and G such that R \in F if and only if -R \in G for all points
    // R \in K. Then the partition is F.
    point_t *partition;
    fp2_element_t *gx;
    fp2_element_t *gy;
    fp2_element_t *u;
    fp2_element_t *v;
    elliptic_curve_t domain;
    elliptic_curve_t codomain;
    long partition_size;
    long kernel_size;
} isogeny_struct;

typedef isogeny_struct isogeny_t[1];

/**
 * Initializes the isogeny {@code isogeny}.
 * @param isogeny
 * @param kernel_size
 */
void sidh_isogeny_init(isogeny_t isogeny,
                       long kernel_size);

/**
 * Frees the memory allocated to {@code isogeny}.
 * @param isogeny
 */
void sidh_isogeny_clear(isogeny_t isogeny);

/**
 * Computes the isogeny from the kernel generated by {@code kernel_gen}.
 * @param isogeny
 * @param kernel_gen
 */
void sidh_isogeny_compute(isogeny_t isogeny,
                          const point_t kernel_gen);

/**
 * Evaluates {@code isogeny} at the point {@code P}, using Velu's formulas.
 * @param Q The result of the evaluation {@code isogeny(P)}
 * @param isogeny
 * @param P
 */
void sidh_isogeny_evaluate_velu(point_t Q,
                                const isogeny_t isogeny,
                                const point_t P);

/**
 * Evaluates {@code isogeny} at the point {@code P}, using Kohel's formulas.
 * @param Q The result of the evaluation {@code isogeny(P)}
 * @param isogeny
 * @param P
 */
void sidh_isogeny_evaluate_kohel(point_t Q,
                                 const isogeny_t isogeny,
                                 const point_t P);

/**
 * Computes the partition for the isogeny generated by {@code kernel_gen}.
 * @see isogeny_struct.
 * @param partition
 * @param kernel_gen
 * @param E
 */
void sidh_isogeny_partition_kernel(point_t *partition,
                                   const point_t kernel_gen,
                                   const elliptic_curve_t E);

/**
 * Checks if {@code R} should be added to the list {@code points}.
 * @param points
 * @param R
 * @param length
 * @return 1 if {@code R} should be added to {@code points}, 0 otherwise
 */
int sidh_isogeny_partition_should_add(point_t *points,
                                      const point_t R,
                                      long length);
/**
 * Sets the kernel size for {@code isogeny}. The new kernel size is assumed
 * to be smaller than the current kernel size.
 * @param isogeny
 * @param kernel_size
 */
void sidh_isogeny_set_kernel_size(isogeny_t isogeny,
                                  long kernel_size);

/**
 * Computes the images of the elliptic curve {@code E} and the points 
 * {@code points} through the isogeny with kernel generated by the point
 * {@code kernel_gen}. The size of the kernel is {@code l^e}. 
 * @param E
 * @param points
 * @param num_points
 * @param kernel_gen
 * @param l
 * @param e the length of the chain of l-isogenies
 * @param isogeny_jump the number of successive l-isogenies that should 
 * be computed at once. For example, if {@code isogeny_jump = 2} then a
 * chain of l-isogenies of length e is computed by doing e / 2 {l^2-isogenies}.
 */
void sidh_isogeny_evaluate_naive(elliptic_curve_t E,
                                 point_t *points,
                                 long num_points,
                                 const point_t kernel_gen,
                                 long l,
                                 long e,
                                 long isogeny_jump);

/**
 * Computes the images of the elliptic curve {@code E} through the isogeny
 * with kernel generated by the point {@code kernel_gen}. 
 * {@link isogeny_evaluate_naive}
 * @param E
 * @param kernel_gen
 * @param l
 * @param e
 * @param isogeny_jump
 */
void sidh_isogeny_evaluate_naive_curve(elliptic_curve_t E,
                                       const point_t kernel_gen,
                                       long l,
                                       long e,
                                       long isogeny_jump);

/**
 * A helper method for {@link isogeny_evaluate_naive}. All the arguments except
 * {@code num_points, le} will be pushed through the isogeny. For example
 * {@code E} will be the codomain of the isogeny. This method should not be
 * called directly.
 * @param isogeny
 * @param E
 * @param points
 * @param num_points
 * @param kernel_gen
 * @param le
 */
void sidh_isogeny_evaluate_naive_helper(isogeny_t isogeny,
                                        elliptic_curve_t E,
                                        point_t *points,
                                        long num_points,
                                        point_t kernel_gen,
                                        const mpz_t le);


/**
 * The recursion for {@link isogeny_evaluate_strategy}.
 * @param E
 * @param points see {@link isogeny_evaluate_strategy}
 * @param num_points see {@link isogeny_evaluate_strategy}
 * @param kernel_gens contains the previous kernels computed while going down
 * the recursion tree.
 * @param num_gens number of elements in {@code kernel_gens}
 * @param l
 * @param e
 * @param ratio see {@link isogeny_evaluate_strategy}
 */
void sidh_isogeny_evaluate_strategy_rec(elliptic_curve_t E,
                                        point_t *points,
                                        long num_points,
                                        point_t *kernel_gens,
                                        long num_gens,
                                        long l,
                                        long e,
                                        float ratio);


/**
 * This method implements the optimal strategy approach proposed in the paper
 * De Feo, Luca, David Jao, and Jérôme Plût. "Towards quantum-resistant 
 * cryptosystems from supersingular elliptic curve isogenies".
 * @param E
 * @param points the points to be evaluated through the isogeny
 * @param num_points number of points in {@code points}
 * @param kernel_gen the generator of the kernel of the isogeny
 * @param l
 * @param e
 * @param ratio a float in the range (0, 1). This indicates the portions of
 * the computation that is done through point multiplication and isogeny 
 * evaluation. The larger values of {@code ratio} means more multiplication
 * and less isogeny evaluation.
 */
void sidh_isogeny_evaluate_strategy(elliptic_curve_t E,
                                    point_t *points,
                                    long num_points,
                                    const point_t kernel_gen,
                                    long l,
                                    long e,
                                    float ratio);

/**
 * The same as {@link isogeny_evaluate_strategy} except there is no point
 * to evaluate through the isogeny. This method simply calls
 * {@link isogeny_evaluate_strategy} with {@code points = NULL, num_points = 0}.
 * @param E
 * @param kernel_gen
 * @param l
 * @param e
 * @param ratio
 */
void sidh_isogeny_evaluate_strategy_curve(elliptic_curve_t E,
                                          const point_t kernel_gen,
                                          long l,
                                          long e,
                                          float ratio);

#ifdef __cplusplus
}
#endif

#endif /* ISOGENY_H */
