import { Navigate } from "react-router-dom";

interface ProtectedRouteProps {
  children: JSX.Element;
}

export default function ProtectedRoute({ children }: ProtectedRouteProps) {
  let accessToken = localStorage.getItem('accessToken');

  if (!accessToken) {
    accessToken = sessionStorage.getItem('accessToken');
  }

  if (!accessToken) {
    return <Navigate to="/login" replace />;
  }

  return children;
}
