// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file exercises.cpp
 * @brief Quick-start aggregate computing exercises.
 */

// [INTRODUCTION]
//! Importing the FCPP library.
#include "lib/fcpp.hpp"

/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp
{

    //! @brief Dummy ordering between positions (allows positions to be used as secondary keys in ordered tuples).
    template <size_t n>
    bool operator<(vec<n> const &, vec<n> const &)
    {
        return false;
    }

    //! @brief Namespace containing the libraries of coordination routines.
    namespace coordination
    {

        //! @brief Tags used in the node storage.
        namespace tags
        {
            //! @brief Color of the current node.
            struct node_color
            {
            };
            //! @brief Size of the current node.
            struct node_size
            {
            };
            //! @brief Shape of the current node.
            struct node_shape
            {
            };

            //! @brief The maximum number of neighbour devices ever witnessed by the current device.
            struct max_seen_devices
            {
            };

            //! @brief The maximum number of neighbour devices ever witnessed by any device in the network.
            struct net_max_seen_devices
            {
            };

            //! @brief Number of neighbors
            struct neighbors
            {
            };

            //! @brief Vector away from neighbor with most neighbors
            struct vec_from_max
            {
            };
            //! @brief Vector to neighbor with least neighbors
            struct vec_to_min
            {
            };

            struct min_ngb_device
            {
            };
            struct max_ngb_device
            {
            };

            //! @brief Generic field for debugging purposes
            struct debug_field
            {
            };

            struct gps_traces
            {
            };

            // ... add more as needed, here and in the tuple_store<...> option below
        }

        //! @brief The maximum communication range between nodes.
        constexpr size_t communication_range = 100;

        // [AGGREGATE PROGRAM]

        /**
         * BASE EXERCISES:
         *
         * Expand the MAIN function below to compute the following:
         *
         * 1)    The number of neighbour devices.
         *
         * 2)    The maximum number of neighbour devices ever witnessed by the current device.
         *
         * 3)    The maximum number of neighbour devices ever witnessed by any device in the network.
         *
         * 4)    Move towards the neighbour with the lowest number of neighbours.
         *
         * Every exercise above is designed to help solving the following one.
         *
         *
         * SIMULATION PHYSICS:
         *
         * 5)    Move away from the neighbour with the highest number of neighbours.
         *
         * 6)    Move as if the device was attracted by the neighbour with the lowest number of neighbours,
         *       and repulsed by the neighbour with the highest number of neighbours.
         *
         * 7)    Move as if the device was repulsed by every neighbour, and by the four walls of the
         *       rectangular box between points [0,0] and [500,500].
         *
         *
         * COMBINING SPATIAL COMPUTING BLOCKS:
         *
         * 8)   Select a node called "source", chosen by finding the node with minimum uid
         *      in the network, assuming that the diameter of the network is no more than 10 hops.
         *
         * 9)   Compute the distances between any node and the "source" using the adaptive bellman-ford algorithm.
         *
         * 10)  Calculate in the source an estimate of the true diameter of the network
         *      (the maximum distance of a device in the network).
         *
         * 11)  Broadcast the diameter to every node in the network.
         *
         *
         * RUNTIME MONITORING:
         *
         * Given that:
         * - the node(s) identified as "source" in exercise (8) are Internet Gateways (gateway),
         * - a node is at risk of disconnection (disrisk) iff it has less than three neighbours,
         * monitor the following properties:
         *
         * 12)  You (the current device) have never been at disrisk.
         *
         * 13)  In the network, there exists a node that has never been at disrisk.
         *
         * 14)  You (the current device) can always reach a gateway through nodes that are not at disrisk.
         *
         * 15)  You (the current device) can always reach a gateway through nodes that have never been at disrisk.
         *
         * In order to check whether what you computed is correct, you may display the computed
         * quantities as node qualities through tags `node_color`, `node_size` and `node_shape`.
         * You can also save your computed quantities in additional specific node attributes:
         * towards this end, you should both add a tag in namespace tags above, then list it
         * (together with the corresponding data type) in the `tuple_store` option below.
         *
         * HINTS:
         *
         * -    In the first few exercises, start by reasoning on when/where to use `nbr` (collecting from
         *      neighbours) and `old` (collecting from the past).
         *
         * -    In order to move a device, you need to set a velocity vector through something like
         *      `node.velocity() = make_vec(0,0)`.
         *
         * -    Coordinates are available through `node.position()`. Coordinates can be composed as physical
         *      vectors: `[1,3] + [2,-1] == [3,2]`, `[2,4] * 0.5 == [1,2]`.
         *
         * -    In the simulation physics exercises, you can model attraction/repulsion using the classical inverse square law.
         *      More precisely, if `v` is the vector between two objects, the resulting force is `v / |v|^3` where
         *      `|v| = sqrt(v_x^2 + v_y^2)`. In FCPP, `norm(v)` is available for computing `|v|`.
         *
         * -    FCPP provides some built-in APIs, like "diameter_election" and "abf_distance".
         *      Refer to the documentation: https://fcpp-doc.surge.sh
         */

        /*
         * @brief example function for checking a property.
         * Sample property: you (the current device) have not been at disrisk for a couple of rounds.
         */
        FUN bool recent_dis_monitor(ARGS, bool disrisk)
        {
            CODE using namespace logic;
            bool prev_disrisk = Y(CALL, disrisk);
            return !disrisk & !prev_disrisk;
        }
        FUN_EXPORT monitor_t = export_list<past_ctl_t, slcs_t>;

        // @brief Main function.
        MAIN()
        {
            // import tag names in the local scope.
            using namespace tags;

            auto& trace = node.net.storage(gps_traces{});

            if(!follow_track(CALL, trace)) {
                node.velocity() = make_vec(0, 0);
            }
            
            node.storage(node_size{}) = 5;
            node.storage(node_color{}) = color(GREEN);
            node.storage(node_shape{}) = shape::sphere;
        }
        //! @brief Export types used by the main function (update it when expanding the program).
        FUN_EXPORT main_t = export_list<double, int, monitor_t, std::tuple<int, vec<2>>>;

    } // namespace coordination

    // [SYSTEM SETUP]

    //! @brief Namespace for component options.
    namespace option
    {

        //! @brief Import tags to be used for component options.
        using namespace component::tags;
        //! @brief Import tags used by aggregate functions.
        using namespace coordination::tags;

        //! @brief Number of people in the area.
        constexpr int node_num = 10;
        //! @brief Dimensionality of the space.
        constexpr size_t dim = 2;

        //! @brief Description of the round schedule.
        using round_s = sequence::periodic<
            distribution::interval_n<times_t, 0, 1>,    // uniform time in the [0,1] interval for start
            distribution::weibull_n<times_t, 10, 1, 10> // weibull-distributed time for interval (10/10=1 mean, 1/10=0.1 deviation)
            >;
        //! @brief The sequence of network snapshots (one every simulated second).
        using log_s = sequence::periodic_n<1, 0, 1>;
        //! @brief The sequence of node generation events (node_num devices all generated at time 0).
        using spawn_s = sequence::multiple_n<node_num, 0>;
        //! @brief The distribution of initial node positions (random in a 500x500 square).
        using rectangle_d = distribution::rect_n<1, 0, 0, 500, 500>;
        //! @brief The contents of the node storage as tags and associated types.
        using store_t = tuple_store<
            node_color, color,
            node_size, double,
            node_shape, shape,
            max_seen_devices, int,
            net_max_seen_devices, int,
            neighbors, int,
            vec_to_min, vec<2>,
            vec_from_max, vec<2>,
            min_ngb_device, device_t,
            max_ngb_device, device_t>;
        //! @brief The tags and corresponding aggregators to be logged (change as needed).
        using aggregator_t = aggregators<
            node_size, aggregator::mean<double>>;

        //! @brief The general simulation options.
        DECLARE_OPTIONS(list,
                        parallel<false>,                // multithreading enabled on node rounds
                        synchronised<false>,           // optimise for asynchronous networks
                        program<coordination::main>,   // program to be run (refers to MAIN above)
                        exports<coordination::main_t>, // export type list (types used in messages)
                        retain<metric::retain<2, 1>>,  // messages are kept for 2 seconds before expiring
                        round_schedule<round_s>,       // the sequence generator for round events on nodes
                        log_schedule<log_s>,           // the sequence generator for log events on the network
                        spawn_schedule<spawn_s>,       // the sequence generator of node creation events on the network
                        net_store<gps_traces, fcpp::gps_trace>,
                        store_t,                       // the contents of the node storage
                        aggregator_t,                  // the tags and corresponding aggregators to be logged
                        init<
                            x, rectangle_d // initialise position randomly in a rectangle for new nodes
                            >,
                        dimension<dim>,                         // dimensionality of the space
                        connector<connect::fixed<100, 1, dim>>, // connection allowed within a fixed comm range
                        shape_tag<node_shape>,                  // the shape of a node is read from this tag in the store
                        size_tag<node_size>,                    // the size  of a node is read from this tag in the store
                        color_tag<node_color>,                   // the color of a node is read from this tag in the store

                        tail_time_val<99999999999999, 1>,   // Trail duration: effectively infinite (num/den = large/1)
                        tail_granularity<1, 10>,            // Snapshot rate
                        tail_color_val<BLUE>,               // Trail color: blue
                        tail_width_val<5, 1>                // Trail width: 5x node size (num/den = 5/1)
        );

    } // namespace option

} // namespace fcpp

//! @brief The main function.
int main()
{
    using namespace fcpp;

    static fcpp::gps_trace trace("C:/Users/loref/Documents/Unito/Tesi Triennale/Otscher.gpx", 47.86675, 15.16357, 833.15, 10);

    //! @brief The network object type (interactive simulator with given options).
    using net_t = component::interactive_simulator<option::list>::net;
    //! @brief The initialisation values (simulation name).
    auto init_v = common::make_tagged_tuple<option::name, option::gps_traces>("Exercises", trace);
    //! @brief Construct the network object.
    net_t network{init_v};
    //! @brief Run the simulation until exit.
    network.run();
    return 0;
}
