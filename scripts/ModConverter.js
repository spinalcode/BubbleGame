//!MENU-ENTRY:Convert Mods

// This script looks in the "music" folder and converts all of the mod/s3m/xm/it files into PMFs + music.h/cpp.
// Press Ctrl+Enter to run this script or use the menu.

start();

function start(){
    let names = [];
    let promises =
        (dir("music")||[])
        .filter( file=>/\.mod$|\.s3m$|\.xm$|\.it$/i.test(file) )
        .map( (file) => new Promise((ok, nok)=>{
            const name = file.replace(/\..*$/, "");
            APP.spawn(
                "${projectPath}/pmf_player/pmf_converter${executableExt}", 
                {cwd:DATA.projectPath + "/music"}, 
                "-i",
                file,
                "-o",
                name + ".pmf")
                .on("data-err", ex=>APP.error(ex))
                .on("close", hasError=>{
                    try{
                        fs.statSync(DATA.projectPath + "/music/" + name + ".pmf");
                        names.push(name);
                    }catch(ex){
                        APP.error("Could not convert " + name);
                    }
                    ok();
                });
        }));

    Promise.all(promises)
    .then(_=>{
        write("music.h", `
// Generated file - DO NOT EDIT!

#pragma once

#include "pmf_player/PMFSource.hpp"

extern "C" {
    extern const Audio::PMF ${names.join(", ")};
}

`);
        write("music.cpp", 
                names
                .map(name=>`__asm__(".global ${name}\\n${name}:\\n.incbin \\"music/${name}.pmf\\"");`)
                .join("\n")
        );
        log("Music conversion complete!");
    })
    .catch(ex=>{
        log(ex);
    });
}

