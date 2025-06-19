import Link from 'next/link';
export default function home(props) {
    return (
        <div id="loginPage" className="page d-flex justify-content-center align-items-center vh-100">
          <div className="card shadow-sm rounded-4 border-0" style={{ width: '100%', maxWidth: '400px' }}>
            <div className="card-body p-4">
              <h3 className="text-primary text-center mb-4 fw-bold">HealthApp Login</h3>
              <form id="loginForm">
                <div className="mb-3">

                  <label htmlFor ="inputEmail" className="form-label fw-semibold">Email address</label>
                  <input type="email" className="form-control" id="inputEmail" placeholder="admin@healthapp.com" required />
                </div>
                <div className="mb-4">
                  <label htmlFor="inputPassword" className="form-label fw-semibold">Password</label>
                  <input type="password" className="form-control" id="inputPassword" placeholder="••••••••" required />
                </div>
                <div className="d-grid">
                  <button type="submit" className="btn btn-primary fw-semibold">Log In</button>
                </div>
              </form>
            </div>
            <div className="card-footer text-center text-muted small">
          Don't have an account? <Link href="/auth/register">Register here</Link>
        </div>
            <div className="card-footer text-center text-muted small">
               <Link href="/dashboard">
                 i am a developer
               </Link>
            </div>
          </div>
        </div>
    );
}