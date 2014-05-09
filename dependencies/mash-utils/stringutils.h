/*******************************************************************************
* MASH 3D simulator
* 
* Copyright (c) 2014 Idiap Research Institute, http://www.idiap.ch/
* Written by Philip Abbet <philip.abbet@idiap.ch>
* 
* This file is part of mash-simulator.
* 
* mash-simulator is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as
* published by the Free Software Foundation.
* 
* mash-simulator is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with mash-simulator. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/


/** @file   stringutils.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the class 'StringUtils'
*/

#ifndef _MASH_STRINGUTILS_H_
#define _MASH_STRINGUTILS_H_

#include "declarations.h"
#include <string>

#ifdef _WIN32
    #include <Winsock2.h>
#else
    #include <sys/time.h>
#endif


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  String-manipulation class
    //--------------------------------------------------------------------------
    class MASH_SYMBOL StringUtils
    {
    public:
        //----------------------------------------------------------------------
        /// @brief  Convert an integer to a string
        ///
        /// @param  val     The integer
        /// @return         The result string
        //----------------------------------------------------------------------
        static std::string toString(int val);

        //----------------------------------------------------------------------
        /// @brief  Convert an unsigned integer to a string
        ///
        /// @param  val     The unsigned integer
        /// @return         The result string
        //----------------------------------------------------------------------
        static std::string toString(unsigned int val);

        //----------------------------------------------------------------------
        /// @brief  Convert a float to a string
        ///
        /// @param  val     The float
        /// @return         The result string
        //----------------------------------------------------------------------
        static std::string toString(float val);

        //----------------------------------------------------------------------
        /// @brief  Convert a double to a string
        ///
        /// @param  val     The double
        /// @return         The result string
        //----------------------------------------------------------------------
        static std::string toString(double val);

        //----------------------------------------------------------------------
        /// @brief  Convert a timevalue to a string
        ///
        /// @param  val     The timevalue
        /// @return         The result string
        //----------------------------------------------------------------------
        static std::string toString(const timeval& val);

        //----------------------------------------------------------------------
        /// @brief  Convert a string to an integer
        ///
        /// @param  val     The string
        /// @return         The result integer
        //----------------------------------------------------------------------
        static int parseInt(const std::string& val);

        //----------------------------------------------------------------------
        /// @brief  Convert a string to an unsigned integer
        ///
        /// @param  val     The string
        /// @return         The result unsigned integer
        //----------------------------------------------------------------------
        static unsigned int parseUnsignedInt(const std::string& val);

        //----------------------------------------------------------------------
        /// @brief  Convert a string to a float
        ///
        /// @param  val     The string
        /// @return         The result float
        //----------------------------------------------------------------------
        static float parseFloat(const std::string& val);

        //----------------------------------------------------------------------
        /// @brief  Convert a string to a double
        ///
        /// @param  val     The string
        /// @return         The result double
        //----------------------------------------------------------------------
        static double parseDouble(const std::string& val);

        //----------------------------------------------------------------------
        /// @brief  Convert a string to a timevalue
        ///
        /// @param  val     The string
        /// @return         The result timevalue
        //----------------------------------------------------------------------
        static struct timeval parseTimeval(const std::string& val);

        //----------------------------------------------------------------------
        /// @brief  Split a string
        ///
        /// @param  str         The string
        /// @param  delims      The delimiters to search
        /// @param  maxSplits   Maximum number of splits to do, 0 for no limit
        /// @return             The result, in the form of a vector of strings
        //----------------------------------------------------------------------
        static tStringList split(const std::string& str,
                                 const std::string& delims = "\t\n",
                                 unsigned int maxSplits = 0);

        //----------------------------------------------------------------------
        /// @brief  Returns whether a string starts with the pattern passed in
        ///
        /// @param  str         The string
        /// @param  pattern     The pattern to compare with
        /// @param  bLowercase  If true, the beginning of the string will be lower cased
        ///                     before comparison. The pattern should also be in lower
        ///                     case.
        /// @return             'true' if the string starts with the pattern
        //----------------------------------------------------------------------
        static bool startsWith(const std::string& str, const std::string& pattern,
                               bool bLowerCase = true);

        //----------------------------------------------------------------------
        /// @brief  Returns whether a string ends with the pattern passed in
        ///
        /// @param  str         The string
        /// @param  pattern     The pattern to compare with
        /// @param  bLowercase  If true, the end of the string will be lower cased
        ///                     before comparison. The pattern should also be in lower
        ///                     case.
        /// @return             'true' if the string ends with the pattern
        //----------------------------------------------------------------------
        static bool endsWith(const std::string& str, const std::string& pattern,
                             bool bLowerCase = true);

        //----------------------------------------------------------------------
        /// @brief  Lower-case all the characters in the string
        ///
        /// @param  str     The string
        //----------------------------------------------------------------------
        static void toLowerCase(std::string &str);

        //----------------------------------------------------------------------
        /// @brief  Replace all the occurence of 'search' in 'value' by 'replacement'
        //----------------------------------------------------------------------
        static std::string replaceAll(const std::string& value,
                                      const std::string& search,
                                      const std::string& replacement);

        //----------------------------------------------------------------------
        /// @brief  Trim the trailing spaces of a string
        ///
        /// @param  str     The string
        /// @param  spaces  The list of characters considered as spaces
        //----------------------------------------------------------------------
        static std::string rtrim(const std::string& str, const std::string& spaces = " \t");

        //----------------------------------------------------------------------
        /// @brief  Trim the leading spaces of a string
        ///
        /// @param  str     The string
        /// @param  spaces  The list of characters considered as spaces
        //----------------------------------------------------------------------
        static std::string ltrim(const std::string& str, const std::string& spaces = " \t");

        //----------------------------------------------------------------------
        /// @brief  Trim the leading and trailing spaces of a string
        ///
        /// @param  str     The string
        /// @param  spaces  The list of characters considered as spaces
        //----------------------------------------------------------------------
        static std::string trim(const std::string& str, const std::string& spaces = " \t");
    };
}

#endif
