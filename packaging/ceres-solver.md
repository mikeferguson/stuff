### notes from packaging ceres-solver 1.8.0

        wget http://ceres-solver.googlecode.com/files/ceres-solver-1.8.0.tar.gz
        
        mkdir ceres-solver
        cd ceres-solver
        git init
        
        gbp import-orig -u 1.8.0 ../ceres-solver-1.8.0.tar.gz
        dh_make
        
        <update files>

        git-buildpackage --git-ignore-new (-us -uc)

