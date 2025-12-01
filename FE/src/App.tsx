import { BrowserRouter as Router, Routes, Route, Navigate } from "react-router-dom";
import HomePage from "./pages/home-page";
import LoginPage from "./pages/login-page";
import RegisterPage from "./pages/register-page";
import Dashboard from "./pages/dashboard";
import UsersPage from "./pages/users-page";
import SettingPage from "./pages/setting-page";
import ProtectedRoute from "./components/protected-route";
import ForgotPasswordPage from "./pages/forgot-password-page";

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Navigate to ="/login" />} />
        <Route 
          path="/dashboard" 
          element={
            <ProtectedRoute>
              <Dashboard />
            </ProtectedRoute>
          } 
        >
          <Route path="home" element={<HomePage />} />
          <Route path="users" element={<UsersPage />} />
          <Route path="settings" element={<SettingPage />} />
        </Route>
        <Route path="/login" element={<LoginPage />} />
        <Route path="/register" element={<RegisterPage />} />
        <Route path="/forgot-password" element={<ForgotPasswordPage />} />
        {/* <Route path="*" element={<Navigate to="/login" />} /> */}
      </Routes>
    </Router>
  );
}

export default App;
