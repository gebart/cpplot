/**
 * Copyright 2011, 2012 Jonatan Olofsson
 * Copyright 2012 Joakim Gebart
 *
 * This file is part of cpplot.
 *
 * cpplot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cpplot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cpplot.  If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
 * License: Gnu Public license (GPL) v3
 * Author: Joakim Gebart (joakim.gebart@jge.se)
 * Based on Scatter-class:
 * Author: Jonatan Olofsson (jonatan.olofsson@gmail.com)
 * Version: 0.1
 * Based on
 * Author: Yuichi Katori (yuichi.katori@gmail.com)
 * Project:MATPLOT++ (MATLAB-like plotting tool in C++).
 * Version:0.3.13
 ****************************************************************************/

#include "cpplot_common.hpp"
#include "color.hpp"
#include "scatter.hpp"
#include <cassert>

namespace cpplot {
    void Scatter::draw() {
        if(type == _2D) { draw2d(); }
        if(type == _3D) { draw3d(); }
    }
    void Scatter::clear() {
        boost::mutex::scoped_lock l(data_mutex);
        XData.clear();
        YData.clear();
        ZData.clear();
        CData.clear();
    }

    void Scatter::draw2d() {
        boost::mutex::scoped_lock l(data_mutex);

        if (XData.size() < 1 || YData.size() < 1)
        {
            return;
        }
        std::vector<float> rgb;
        const unsigned int np = std::min(XData.size(), YData.size()); // number of points
        if(PointColor != "none") {
            rgb = ColorSpec2RGB(PointColor);
        }
        else
        {
            rgb[0] = 0.f;
            rgb[1] = 0.f;
            rgb[2] = 0.f;
        }
        glPointSize(PointSize);
        glBegin(GL_POINTS);
            for (unsigned int i = 0; i < np; ++i)
            {
                if (CData.size() > i && CData[i].size() > 2)
                {
                    glColor3fv(&CData[i][0]);
                }
                else
                {
                    glColor3fv(&rgb[0]);
                }
                glVertex2d(ctx(XData[i]), cty(YData[i]));
            }
        glEnd();
    }


    void Scatter::draw3d() {
        boost::mutex::scoped_lock l(data_mutex);

        if (XData.size() < 1 || YData.size() < 1 || ZData.size() < 1)
        {
            return;
        }
        std::vector<float> rgb;
        const unsigned int np = std::min(std::min(XData.size(), YData.size()), ZData.size()); // number of points
        if(PointColor != "none") {
            rgb = ColorSpec2RGB(PointColor);
        }
        else
        {
            rgb[0] = 0.f;
            rgb[1] = 0.f;
            rgb[2] = 0.f;
        }
        glPointSize(PointSize);
        glBegin(GL_POINTS);
            for (unsigned int i = 0; i < np; ++i)
            {
                if (CData.size() > i && CData[i].size() > 2)
                {
                    glColor3fv(&CData[i][0]);
                }
                else
                {
                    glColor3fv(&rgb[0]);
                }
                glVertex3d(ct3x(XData[i]), ct3y(YData[i]), ct3z(ZData[i]));
            }
        glEnd();
    }

    /// patch
    scatter_t Scatter::scatter(const dvec& X, const dvec& Y) {
        boost::mutex::scoped_lock l(data_mutex);
        // Single color
        type = _2D;

        XData = X;
        YData = Y;
        ZData.clear();
        CData.clear();

        return shared_from_this();
    }
    scatter_t Scatter::scatter(const dvec& X, const dvec& Y, const dvec& C) {
        boost::mutex::scoped_lock l(data_mutex);
        // One color per face with index color
        type = _2D;
        XData = X;
        YData = Y;
        ZData.clear();
        CData = Index2TrueColor(C);

        return shared_from_this();
    }
    scatter_t Scatter::scatter(const dvec& X, const dvec& Y, const tcvec& C) {
        boost::mutex::scoped_lock l(data_mutex);
        // One color per face with true color
        type = _2D;
        XData = X;
        YData = Y;
        ZData.clear();
        CData = C;

        return shared_from_this();
    }
    //~ scatter_t Scatter::scatter(const dvec& X, const dvec& Y, const dvec& Z) {
        //~ boost::mutex::scoped_lock l(data_mutex);
        //~ // Single color
        //~ ca->type = axes_t_t::_3D;
        //~ type = _3D;
        //~ XData = X;
        //~ YData = Y;
        //~ ZData = Z;
        //~ CData.clear();
//~
        //~ return shared_from_this();
    //~ }
    scatter_t Scatter::scatter(const dvec& X, const dvec& Y, const dvec& Z, const dvec& C) {
        boost::mutex::scoped_lock l(data_mutex);
        // One color per face
        ca->type = axes_t_t::_3D;
        type = _3D;

        XData = X;
        YData = Y;
        ZData = Z;
        CData = Index2TrueColor(C);

        return shared_from_this();
    }
    scatter_t Scatter::scatter(const dvec& X, const dvec& Y, const dvec& Z, const tcvec& C) {
        boost::mutex::scoped_lock l(data_mutex);
        // One color per face
        ca->type = axes_t_t::_3D;
        type = _3D;

        XData = X;
        YData = Y;
        ZData = Z;
        CData = C;

        return shared_from_this();
    }

    void Scatter::config() {
        boost::mutex::scoped_lock l(data_mutex);
        ca->xmax = std::max( math::max(XData), ca->xmax );
        ca->xmin = std::min( math::min(XData), ca->xmin );
        ca->ymax = std::max( math::max(YData), ca->ymax );
        ca->ymin = std::min( math::min(YData), ca->ymin );
        ca->zmax = std::max( math::max(ZData), ca->zmax );
        ca->zmin = std::min( math::min(ZData), ca->zmin );
    }

    scatter_t Scatter::set(const std::string p, const std::string v) {
             if(p == "COLOR")    { PointColor = v; }
        else if(p == "PointColor") { PointColor = v; }

        return shared_from_this();
    }

    scatter_t Scatter::set(const std::string p, const float v) {
        if(p == "PointSize") { PointSize = v; }
        return shared_from_this();
    }

    tcvec Scatter::Index2TrueColor(const dvec& IC) {
        if(ca->CLim[0] == ca->CLim[1]) {
            ca->CLim[0] = math::min(IC);
            ca->CLim[1] = math::max(IC);
            //~ ca->CLim[0] = std::min( math::min(IC), math::min(IC) ); ???????WHAT????
            //~ ca->CLim[1] = std::max( math::max(IC), math::max(IC) );
        }
        std::vector<float> rgb;
        tcvec tc;
        for(dvec::const_iterator it = IC.begin(); it != IC.end(); ++it) {
            rgb = ca->map2color(*it);
            tc.push_back(rgb);
        }
        return tc;
    }
}
