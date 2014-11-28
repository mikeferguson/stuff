### argparse with roslaunch
roslaunch sends a number of other parameters, which will cause the regular parse_args() to fail. Use:

        args, unknown = parser.parse_known_args()
