import re
Import("env")

remove_libs = ["json"]

print "Preventing espidf libraries from compiling"

# remove json libraries in remove_libs
libs = env["LIBS"]

def removeLibs(arr, removeLibs):
    removed = False
    for lib in libs:
        for r in removeLibs:
            if str(lib).find(r) != -1:
                removed = True
                arr.remove(lib)
                removeLibs.remove(r)
                print("- %s"%r)
                break
        if len(removeLibs) == 0:
            break
    return removed

not_found = list(remove_libs) 
removed = removeLibs(libs, not_found)

for lib in not_found:
    print "- '%s' not found"%lib
if not removed:
    print "- No libraries removed"

# remove include dir
cpppath = env["CPPPATH"]

print "Removing espidf library include directories"

def removeIncs(arr, removeLibs):
    removed = False
    for incDir in arr:
        for r in removeLibs:
            if re.search(".*components[/\\\\]+%s.*"%r, str(incDir)) != None:
                arr.remove(incDir)
                print("- %s"%incDir)
                removeLibs.remove(r)
                removed = True
                break
        if len(removeLibs) == 0:
            break
    return removed

not_found = list(remove_libs)
removed = removeIncs(cpppath, not_found)

for lib in not_found:
    print "- '%s' not found"%lib
if not removed:
    print "- No include directories removed"

print

env.Replace(LIBS = libs, CPPPATH = cpppath)