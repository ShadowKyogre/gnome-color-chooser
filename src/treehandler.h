/* GNOME Color Chooser - GTK+/GNOME desktop appearance customization tool
 * Copyright (C) 2006-2008 Werner Pantke <wpantke@punk-ass-bitch.org>
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
 *
 * Project email: <gnome-color-chooser@punk-ass-bitch.org>
 *
 */

#ifndef _TREEHANDLER_H
#define _TREEHANDLER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>

using std::string;


class TreeHandler
{
  public:
    TreeHandler(char* file);
    ~TreeHandler();
    void save();
    void reset_to_defaults();
    
//    int getConfigVersion();
//    void setConfigVersion(int version);
    
    void addCategory(xmlNode* category);
    void addElement(xmlNode* element);
    void addClass(xmlNode* cl);
    void attachEngine(xmlNode* engine, char* cat_name);
    void unattachEngine(string category);
    void setProfile(xmlNode *profile);
    void unsetProfile();
    bool hasProfile();
    const string getProfile();
    const string getProfileName();

    int getCategoryCount();
    xmlNode* getCategoryNode();
    xmlNode* getElementNode();
    xmlNode* getClassNode();

    void gotoFirstCategory();
    void gotoNextCategory();
    bool getCategoryOverride();
    const string getCategoryName();
    const string getCategoryLongName();
    const string getCategoryShortDescription();
    int getCategoryElementsCount();
    int getCategoryClassesCount();
    void gotoCategoriesFirstElement();
    void gotoCategoriesNextElement();

    void gotoCategoriesEngine();
    void gotoCategoriesEngine(string category);
    void gotoCategoryEnginesFirstParam();
    void gotoCategoryEnginesNextParam();
    void gotoCategoryEnginesParamByName(char* name);
    int getCategoryEnginePropertiesCount();
    const string getEngineName();
    const string getEngineName(string category);
    const string getParamName();
    const string getParamValue();
    const string getParamType();
    const string getParamLongName();
    const string getParamShortDescription();

    bool getEngineOverride();
    bool getEngineOverride(string category);
    void setEngineOverride(string category, bool override);
    string getEngineMetaInfo(string category);
    bool getParamOverride();
    //void setParamOverride(string category, string id, bool override);

    void gotoFirstElement();
    void gotoNextElement();
    void gotoElementById(int id);
    int getElementCount(); // overwrites current category and current element !!
    bool getElementOverride();
    const string getElementIdString();
    const string getElementName();
    const string getElementLongName();
    const string getElementValue();
    const string getElementType();
    int   getElementMin();
    int   getElementMax();
    const string getElementShortDescription();
    
    void  setElementValue(const char* value);
        
    void gotoCategoriesFirstClass();
    void gotoCategoriesNextClass();
    const string getClassName();
    const string getClassType();

    const string getValue(int id);
    void  setValue(int id, const char* value);
    const string getValue(string category, string id);
    void  setValue(string category, string id, const char* value);

    
    bool  getOverride(int id); // element
    bool  getOverride(string category, string id); // engine param

    void  setOverride(int id, bool override); // element
    void  setOverride(string category, string id, bool override); // engine param
    //char* getPath(int id);
    //void  setPath(int id, char* value);

    /* get-only params */
    int getMin(int id);
    int getMax(int id);
    int getMin(string category, string id);
    int getMax(string category, string id);
    const string getChecked(string category, string id);
    const string getUnchecked(string category, string id);
    int getFractionalDigits(string category, string id);
    const string getType(string category, string id);


  private:
    char* filename;
//    int version;
    xmlDoc *doc;
    xmlNode *root_node;
    xmlNode *cat_cur;
    xmlNode *el_cur;
    xmlNode *class_cur;
    xmlNode *param_cur;
    xmlNode *eng_cur;


    void setMaxCatId(int max);
    void setMaxElementId(int max);
    xmlNode* getElementById(int id);
    void setOverride(xmlNode* element, bool override);

    xmlNode* get_category(string name);
    xmlNode* get_first_category();
    xmlNode* get_next_category(xmlNode* category);
    xmlNode* get_engine(string category_name);
    xmlNode* get_engine(xmlNode* category);
    xmlNode* get_profile();
    xmlNode* get_param(xmlNode* engine, string name);
    xmlNode* get_first_param(xmlNode* engine);
    xmlNode* get_next_param(xmlNode* param);

};

#endif // _TREEHANDLER_H

