import { useState, useEffect } from 'react'
import { WAForthShell } from './forth'

import './App.css'

function App() {
  const [count, setCount] = useState(0);

  return (
    <>
        <WAForthShell />
    </>
  )
}

export default App
