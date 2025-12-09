import { Box, Button, Paper, TextField, Typography } from "@mui/material"
import { useState } from "react"
import bg from "../assets/background.jpg";
import { useNavigate } from "react-router-dom";
import apiClient from "../api/axiosConfig";
import { AxiosError } from "axios";
import IconButton from '@mui/material/IconButton';
import InputAdornment from '@mui/material/InputAdornment';
import Visibility from '@mui/icons-material/Visibility';
import VisibilityOff from '@mui/icons-material/VisibilityOff';

const RegisterPage = () => {
    const [name, setName] = useState("");
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");
    const [confirmPassword, setConfirmPassword] = useState("");
    const [error, setError] = useState("");
    const [showPassword, setShowPassword] = useState(false);
    const navigate = useNavigate();

    const handleRegister = async (e) => {
        e.preventDefault();
        setError("");

        if (password !== confirmPassword) {
            setError("Mật khẩu không khớp, vui lòng nhập lại.");
            return;
        }

        try {
            await apiClient.post('/auth/register', {
                name,
                email,
                password,
            })

            navigate('/login');
        } catch (err) {
            let errorMessage = "Đã xảy ra lỗi. Vui lòng thử lại.";

            if (err instanceof AxiosError) {
            
            // Kiểm tra phản hồi từ server
                if (err.response && err.response.data && err.response.data.message) {
                    
                    const serverMessage = err.response.data.message;
                    
                    // Xử lý trường hợp message là mảng (từ ValidationPipe)
                    errorMessage = Array.isArray(serverMessage)
                        ? serverMessage.join(', ')
                        : serverMessage;
                }
            }
            setError(errorMessage);
            console.log("Error object:", err);
        }
    }

    return (
        <Box
            sx={{
                height: "100vh",
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                backgroundImage: `url(${bg})`,
                backgroundSize: "cover",
                backgroundPosition: "center",
            }}
        >
            <Paper
                elevation={10}   //tao hieu ung do bong
                sx={{
                    p: 5,
                    width: 550,
                    borderRadius: 4,
                    backgroundColor: "rgba(255, 255, 255, 0.98)",
                }}
            >
                <Typography variant="h4" align="center" mb={3} fontWeight="bold">
                    Đăng ký
                </Typography>

                <form onSubmit={handleRegister}>
                    <TextField
                        label="Tên"
                        type="name"
                        value={name}
                        onChange={(e) => setName(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                    />
                    <TextField
                        label="Email"
                        type="email"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                    />
                    <TextField
                        label="Mật khẩu"
                        type="password"
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                        InputProps={{
                            endAdornment: (
                                <InputAdornment position="end">
                                    <IconButton
                                        onClick={() => setShowPassword(!showPassword)}
                                        edge="end"
                                    >
                                        {showPassword ? <Visibility /> : <VisibilityOff />}
                                    </IconButton>
                                </InputAdornment>
                            ),
                        }}
                    />

                    <TextField
                        label="Nhập lại Mật khẩu"
                        type="password"
                        value={confirmPassword}
                        onChange={(e) => setConfirmPassword(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                    />

                    <Button
                        type="submit"
                        variant="contained"
                        color="primary"
                        fullWidth
                        sx={{ 
                            mt: 2, 
                            py: 1.2,    
                            borderRadius: 2,
                        }}
                    >
                        Đăng ký
                    </Button>
                </form>

                {error && (
                    <Typography color="error" variant="body2" align="center" sx={{ mt: 1, mb: 1 }}>
                        {error}
                    </Typography>
                )}

                <Typography
                    variant="body2"
                    align="center"
                    sx={{ mt: 3, color: "text.secondary" }}
                >
                    Đã có tài khoản?{" "}
                    <a href="/login" style={{ textDecoration: "none", color: "#1976d2" }}>
                        Đăng nhập ngay
                    </a>
                </Typography>
            </Paper>
        </Box>
    )
}

export default RegisterPage