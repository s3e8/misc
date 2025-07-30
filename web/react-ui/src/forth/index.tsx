import { useState, useEffect, useRef, useCallback } from 'react';

import "./shell/shell.css";
import { WAForthConsole } from "./shell/Console-react";

export function WAForthShell() {
    // const containerRef = useRef<HTMLDivElement>(null);
    // useEffect(() => {
    //     if (containerRef.current) {
    //         containerRef.current.innerHTML = ''; // temp solution for preventing adjacent duplicate renderings
    //         renderConsole();
    //     }
    // }, []);

    return (
        <WAForthConsole />
    );
}

// export function renderWAForthNotebook() {}
// export function renderWAForthTurtleEditor() {}
// export function doForth() {}