import Link from 'next/link'

export default function Signup() {
  return (
    <div id="signupPage" className="page d-flex justify-content-center align-items-center vh-100">
      <div className="card shadow-sm rounded-4 border-0" style={{ width: '100%', maxWidth: '400px' }}>
        <div className="card-body p-4">
          <h3 className="text-primary text-center mb-4 fw-bold">HealthApp Sign Up</h3>
          <form id="signupForm">
            <div className="mb-3">
              <label htmlFor="inputName" className="form-label fw-semibold">Full Name</label>
              <input type="text" className="form-control" id="inputName" placeholder="Name" required />
            </div>
            <div className="mb-3">
              <label htmlFor="inputEmail" className="form-label fw-semibold">Email address</label>
              <input type="email" className="form-control" id="inputEmail" placeholder="user@healthapp.com" required />
            </div>
            <div className="mb-3">
              <label htmlFor="inputPassword" className="form-label fw-semibold">Password</label>
              <input type="password" className="form-control" id="inputPassword" placeholder="••••••••" required />
            </div>
            <div className="mb-4">
              <label htmlFor="inputConfirmPassword" className="form-label fw-semibold">Confirm Password</label>
              <input type="password" className="form-control" id="inputConfirmPassword" placeholder="••••••••" required />
            </div>
            <div className="d-grid">
              <button type="submit" className="btn btn-primary fw-semibold">Sign Up</button>
            </div>
          </form>
        </div>
        <div className="card-footer text-center text-muted small">
          Already have an account? <Link href="/auth/login">Login here</Link>
        </div>
      </div>
    </div>
  )
}
