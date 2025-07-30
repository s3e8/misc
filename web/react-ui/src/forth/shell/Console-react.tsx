import { useEffect, useState, useCallback } from 'react';
import WAForth, { withCharacterBuffer } from 'waforth';

export function WAForthConsole() {
    const [forth] = useState(() => new WAForth());
    const [isLoading, setIsLoading] = useState(true);
    const [output, setOutput] = useState('');
    const [inputBuffer, setInputBuffer] = useState<string[]>([]);
    const [inputElVisibility, setInputElVisibility] = useState('hidden');

    useEffect(() => {
        forth.onEmit = withCharacterBuffer((c) => {
            setOutput(prev => prev + c); // why not prev => c ???
        })


    }, [forth]);

    function handleConsoleClick() {
        setInputElVisibility(true);
    }

    return (
        <div className="waforth-shell">
            <pre className="Console" onClick={(e) => {handleConsoleClick()}}>

            </pre>
        </div>
    );
}