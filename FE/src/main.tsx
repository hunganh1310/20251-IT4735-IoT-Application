import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import App from './App.tsx'
import { CssBaseline, GlobalStyles } from '@mui/material'
import { ToastContainer } from 'react-toastify'

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <CssBaseline/>
    <GlobalStyles  styles={{ body: { margin: 0, padding: 0 }} }/>
    <App />
  </StrictMode>,
)
